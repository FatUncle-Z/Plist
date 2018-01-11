//
//  PlistBinaryWriter.cpp
//  Plist
//
//  Created by zhaojun on 2017/8/11.
//  Copyright © 2017年 zhaojun. All rights reserved.
//

#include <fstream>
#include <codecvt>

#include "PlistBinaryWriter.h"
#include "PlistBinaryHelper.h"
#include "../utils/PlistUtils.h"

namespace Plist
{
    void PlistBinaryWriter::writePlistBinary(Plist::PlistHelperData &d, const Plist::Object &message)
    {
        //int totalRefs =  countDictionary(message);
        int totalRefs =  countAny(message) - 1;
        d._refCount = totalRefs;
        
        d._objRefSize = PlistBinaryHelper::regulateNullBytes(PlistBinaryHelper::intToBytes<int32_t>(d._refCount, PlistBinaryHelper::hostLittleEndian()), 1).size();
        
        //writeBinaryDictionary(d, message);
        writeBinary(d, message);
        writeBinaryString(d, "bplist00", false);
        d._offsetTableOffset = (int64_t) d._objectTable.size();
        d._offsetTable.push_back(d._objectTable.size() - 8);
        d._offsetByteSize = PlistBinaryHelper::regulateNullBytes(PlistBinaryHelper::intToBytes<int>(d._offsetTable.back(), PlistBinaryHelper::hostLittleEndian()), 1).size();
        
        std::vector<unsigned char> offsetBytes;
        
        reverse(d._offsetTable.begin(), d._offsetTable.end());
        
        for(unsigned int i = 0; i < d._offsetTable.size(); ++i)
        {
            d._offsetTable[i] = d._objectTable.size() - d._offsetTable[i];
            std::vector<unsigned char> buffer = PlistBinaryHelper::regulateNullBytes(PlistBinaryHelper::intToBytes<int>(d._offsetTable[i], PlistBinaryHelper::hostLittleEndian()), d._offsetByteSize);
            //reverse(buffer.begin(), buffer.end());
            
            offsetBytes.insert(offsetBytes.end(), buffer.rbegin(), buffer.rend());
        }
        
        d._objectTable.insert(d._objectTable.end(), offsetBytes.begin(), offsetBytes.end());
        
        std::vector<unsigned char> dummy(6, 0);
        d._objectTable.insert(d._objectTable.end(), dummy.begin(), dummy.end());
        d._objectTable.push_back((unsigned char) (d._offsetByteSize));
        d._objectTable.push_back((unsigned char) (d._objRefSize));
        std::vector<unsigned char> temp = PlistBinaryHelper::intToBytes<int64_t>((int64_t) totalRefs + 1, PlistBinaryHelper::hostLittleEndian());
        d._objectTable.insert(d._objectTable.end(), temp.rbegin(), temp.rend());
        temp = PlistBinaryHelper::intToBytes<int64_t>(0, PlistBinaryHelper::hostLittleEndian());
        d._objectTable.insert(d._objectTable.end(), temp.begin(), temp.end());
        temp = PlistBinaryHelper::intToBytes<int64_t>(d._offsetTableOffset, PlistBinaryHelper::hostLittleEndian());
        d._objectTable.insert(d._objectTable.end(), temp.rbegin(), temp.rend());
    }
    
    int PlistBinaryWriter::countAny(const Plist::Object &object)
    {
        int count = 0;
        if(object.getType() == typeid(Dictionary))
            count += countDictionary(obj_cast<Dictionary >(object)) + 1;
        else if (object.getType() == typeid(Array))
            count += countArray(obj_cast<Array >(object)) + 1;
        else
            ++count;
        return count;
    }
    
    int PlistBinaryWriter::countArray(const Array &array)
    {
        int count = 0;
        for(Array::const_iterator it = array.begin();
            it != array.end();
            ++it)
            count += countAny(*it);
        
        return count;
    }
    
    int PlistBinaryWriter::countDictionary(const Dictionary &dictionary)
    {
        int count = 0;
        for(Dictionary::const_iterator it = dictionary.begin();
            it != dictionary.end();
            ++it)
        {
            ++count;
            count += countAny(it->second);
        }
        
        return count;
    }
    
    std::vector<unsigned char> PlistBinaryWriter::writeBinaryDictionary(Plist::PlistHelperData &d, const Dictionary &dictionary)
    {
        
        std::vector<int32_t> refs;
        for(Dictionary::const_reverse_iterator it = dictionary.rbegin();
            it != dictionary.rend();
            ++it)
        {
            writeBinary(d, it->second);
            d._offsetTable.push_back(d._objectTable.size());
            refs.push_back(d._refCount);
            d._refCount--;
        }
        
        for(Dictionary::const_reverse_iterator it = dictionary.rbegin();
            it != dictionary.rend();
            ++it)
        {
            writeBinary(d, it->first);
            d._offsetTable.push_back(d._objectTable.size());
            refs.push_back(d._refCount);
            d._refCount--;
        }
        
        std::vector<unsigned char> header;
        if (dictionary.size() < 15)
        {
            header.push_back(0xD0 | ((unsigned char) dictionary.size()));
        }
        else
        {
            header.push_back(0xD0 | 0xf);
            std::vector<unsigned char> theSize = writeBinaryInteger(d, dictionary.size(), false);
            header.insert(header.end(), theSize.begin(), theSize.end());
        }
        
        // try to do this more efficiently.  Not good to insert at the begining of buffer.
        
        std::vector<unsigned char> buffer;
        for(std::vector<int32_t>::const_iterator it = refs.begin();
            it != refs.end();
            ++it)
        {
            std::vector<unsigned char> refBuffer = PlistBinaryHelper::regulateNullBytes(PlistBinaryHelper::intToBytes<int32_t>(*it, PlistBinaryHelper::hostLittleEndian()), d._objRefSize);
            //		reverse(refBuffer.begin(), refBuffer.end());
            buffer.insert(buffer.begin(), refBuffer.rbegin(), refBuffer.rend());
        }
        
        buffer.insert(buffer.begin(), header.begin(), header.end());
        
        d._objectTable.insert(d._objectTable.begin(), buffer.begin(), buffer.end());
        
        return buffer;
    }
    
    std::vector<unsigned char> PlistBinaryWriter::writeBinaryArray(Plist::PlistHelperData &d, const Array &array)
    {
        std::vector<int32_t> refs;
        for(Array::const_reverse_iterator it = array.rbegin();
            it != array.rend();
            ++it)
        {
            writeBinary(d, *it);
            d._offsetTable.push_back(d._objectTable.size());
            refs.push_back(d._refCount);
            d._refCount--;
        }
        
        std::vector<unsigned char> header;
        if (array.size() < 15)
        {
            header.push_back(0xA0 | ((unsigned char) array.size()));
        }
        else
        {
            header.push_back(0xA0 | 0xf);
            std::vector<unsigned char> theSize = writeBinaryInteger(d, array.size(), false);
            header.insert(header.end(), theSize.begin(), theSize.end());
        }
        
        // try to do this more efficiently.  Not good to insert at the begining of buffer.
        
        std::vector<unsigned char> buffer;
        for(std::vector<int32_t>::const_iterator it = refs.begin();
            it != refs.end();
            ++it)
        {
            std::vector<unsigned char> refBuffer = PlistBinaryHelper::regulateNullBytes(PlistBinaryHelper::intToBytes<int32_t>(*it, PlistBinaryHelper::hostLittleEndian()), d._objRefSize);
            //		reverse(refBuffer.begin(), refBuffer.end());
            buffer.insert(buffer.begin(), refBuffer.rbegin(), refBuffer.rend());
        }
        
        buffer.insert(buffer.begin(), header.begin(), header.end());
        
        d._objectTable.insert(d._objectTable.begin(), buffer.begin(), buffer.end());
        
        return buffer;
    }
    
    std::vector<unsigned char> PlistBinaryWriter::writeBinaryByteArray(Plist::PlistHelperData &d, const Data &byteArray)
    {
        std::vector<unsigned char> header;
        if(byteArray.size() < 15)
            header.push_back(0x40 | ((unsigned char) byteArray.size()));
        else
        {
            header.push_back(0x40 | 0xf);
            std::vector<unsigned char> theSize = writeBinaryInteger(d, byteArray.size(), false);
            header.insert(header.end(), theSize.begin(), theSize.end());
        }
        
        std::vector<unsigned char> buffer(header);
        buffer.insert(buffer.end(), (unsigned char*) PlistBinaryHelper::vecData(byteArray), (unsigned char*) PlistBinaryHelper::vecData(byteArray) + byteArray.size());
        d._objectTable.insert(d._objectTable.begin(), buffer.begin(), buffer.end());
        
        return buffer;
    }
    
    std::vector<unsigned char> PlistBinaryWriter::writeBinaryInteger(Plist::PlistHelperData &d, int64_t value, bool write)
    {
        using namespace std;
        
        // The integer is initially forced to be 64 bit because it must be serialized
        // as 8 bytes if it is negative.   If it is not negative, the
        // regulateNullBytes step will reduce the representation down to the min
        // power base 2 bytes needed to store it.
        
        vector<unsigned char> buffer = PlistBinaryHelper::intToBytes<int64_t>(value, PlistBinaryHelper::hostLittleEndian());
        buffer = PlistBinaryHelper::regulateNullBytes(PlistBinaryHelper::intToBytes<int64_t>(value, PlistBinaryHelper::hostLittleEndian()), 1);
        buffer.resize(PlistBinaryHelper::nextpow2(buffer.size()), 0);
        
        unsigned char header = 0x10 | PlistBinaryHelper::ilog2(buffer.size());
        buffer.push_back(header);
        reverse(buffer.begin(), buffer.end());
        
        if(write)
            d._objectTable.insert(d._objectTable.begin(), buffer.begin(), buffer.end());
        
        return buffer;
    }
    
    std::vector<unsigned char> PlistBinaryWriter::writeBinaryBool(Plist::PlistHelperData &d, bool value)
    {
        std::vector<unsigned char> buffer;
        if(value)
            buffer.push_back(0x09);
        else
            buffer.push_back(0x08);
        
        d._objectTable.insert(d._objectTable.begin(), buffer.begin(), buffer.end());
        return buffer;
    }
    
    std::vector<unsigned char> PlistBinaryWriter::writeBinaryDate(Plist::PlistHelperData &d, const Plist::Date &date)
    {
        std::vector<unsigned char> buffer;
        
        // need to serialize as Apple epoch.
        
        double macTime = date.timeAsAppleEpoch();
        
        buffer = PlistBinaryHelper::doubleToBytes(macTime, false);
        buffer.insert(buffer.begin(), 0x33);
        
        d._objectTable.insert(d._objectTable.begin(), buffer.begin(), buffer.end());
        return buffer;
    }
    
    std::vector<unsigned char> PlistBinaryWriter::writeBinaryDouble(Plist::PlistHelperData &d, double value)
    {
        std::vector<unsigned char> buffer = PlistBinaryHelper::regulateNullBytes(PlistBinaryHelper::doubleToBytes(value, PlistBinaryHelper::hostLittleEndian()), 4);
        buffer.resize(PlistBinaryHelper::nextpow2(buffer.size()), 0);
        
        unsigned char header = 0x20 | PlistBinaryHelper::ilog2(buffer.size());
        buffer.push_back(header);
        reverse(buffer.begin(), buffer.end());
        
        d._objectTable.insert(d._objectTable.begin(), buffer.begin(), buffer.end());
        
        return buffer;
    }
    
    std::vector<unsigned char> PlistBinaryWriter::writeBinary(Plist::PlistHelperData &d, const Plist::Object &obj)
    {
        const std::type_info &objType = obj.getType();
        
        std::vector<unsigned char> value;
        if(objType == typeid(int32_t))
            value = writeBinaryInteger(d, obj_cast<const int32_t&>(obj), true);
        else if(objType == typeid(int64_t))
            value = writeBinaryInteger(d, obj_cast<const int64_t&>(obj), true);
        else if(objType == typeid(long))
            value = writeBinaryInteger(d, obj_cast<const long&>(obj), true);
        else if(objType == typeid(short))
            value = writeBinaryInteger(d, obj_cast<const short&>(obj), true);
        else if(objType == typeid(Dictionary))
            value = writeBinaryDictionary(d, obj_cast<const Dictionary& >(obj));
        else if(objType == typeid(String))
            value = writeBinaryString(d, obj_cast<const String&>(obj), true);
        else if (objType == typeid(char*))
            value = writeBinaryString(d, PlistUtils::stringFromValue(obj_cast<char*>(obj)), true);
        else if (objType == typeid(const char*))
            value = writeBinaryString(d, PlistUtils::stringFromValue(obj_cast<const char*>(obj)), true);
        else if(objType == typeid(Array))
            value = writeBinaryArray(d, obj_cast<const Array& >(obj));
        else if(objType == typeid(Data))
            value = writeBinaryByteArray(d, obj_cast<const Data& >(obj));
        else if(objType == typeid(double))
            value = writeBinaryDouble(d, obj_cast<const double&>(obj));
        else if(objType == typeid(float))
            value = writeBinaryDouble(d, obj_cast<const float&>(obj));
        else if(objType == typeid(Date))
            value = writeBinaryDate(d, obj_cast<const Date&>(obj));
        else if(objType == typeid(bool))
            value = writeBinaryBool(d, obj_cast<const bool&>(obj));
        else
        {
            throw Error((std::string("Plist Error: Can't serialize type ") + objType.name()).c_str());

        }
        return value;
    }
    
    std::vector<unsigned char> PlistBinaryWriter::writeBinaryString(Plist::PlistHelperData &d, const std::string &value, bool head)
    {
        std::vector<unsigned char> buffer;
        if (HasChinese(value.c_str())) {
            buffer = writeBinaryUnicodeString(d, value, head);
        }
        else {
            buffer = writeBinaryUTF8String(d, value, head);
        }
        
        return buffer;
    }
    
    std::vector<unsigned char> PlistBinaryWriter::writeBinaryUTF8String(Plist::PlistHelperData &d, const std::string &value, bool head)
    {
        std::vector<unsigned char> buffer;
        buffer.reserve(value.size());
        
        for(std::string::const_iterator it = value.begin();
            it != value.end();
            ++it)
            buffer.push_back((unsigned char) *it);
        
        if(head)
        {
            std::vector<unsigned char> header;
            if (value.length() < 15)
                header.push_back(0x50 | ((unsigned char) value.length()));
            else
            {
                header.push_back(0x50 | 0xf);
                std::vector<unsigned char> theSize = writeBinaryInteger(d, buffer.size(), false);
                header.insert(header.end(), theSize.begin(), theSize.end());
            }
            buffer.insert(buffer.begin(), header.begin(), header.end());
        }
        
        d._objectTable.insert(d._objectTable.begin(), buffer.begin(), buffer.end());
        
        return buffer;
    }
    
    std::vector<unsigned char> PlistBinaryWriter::writeBinaryUnicodeString(Plist::PlistHelperData &d, const std::string &value, bool head)
    {
        std::vector<unsigned char> buffer;
        buffer.reserve(value.size());
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
        std::wstring wstr = converter.from_bytes(value);
        for(std::wstring::const_iterator it = wstr.begin(); it != wstr.end(); ++it) {
            buffer.push_back(*it>>8);
            buffer.push_back(*it);
        }
        
        if(head)
        {
            std::vector<unsigned char> header;
            if (value.length() < 15)
                header.push_back(0x60 | ((unsigned char) wstr.length()));
            else
            {
                header.push_back(0x60 | 0xf);
                std::vector<unsigned char> theSize = writeBinaryInteger(d, buffer.size(), false);
                header.insert(header.end(), theSize.begin(), theSize.end());
            }
            buffer.insert(buffer.begin(), header.begin(), header.end());
        }
        
        d._objectTable.insert(d._objectTable.begin(), buffer.begin(), buffer.end());
        
        return buffer;
    }
    
    
    void Plist::PlistBinaryWriter::writePlistBinary(std::ostream &stream, const Plist::Object &message)
    {
        PlistHelperData d;
        writePlistBinary(d, message);
        stream.write((const char*) PlistBinaryHelper::vecData(d._objectTable), d._objectTable.size());
    }
    
    void PlistBinaryWriter::writePlistBinary(Data &plist, const Plist::Object &message)
    {
        PlistHelperData d;
        writePlistBinary(d, message);
        plist.resize(d._objectTable.size());
        std::copy((const char*) PlistBinaryHelper::vecData(d._objectTable), (const char*) PlistBinaryHelper::vecData(d._objectTable) + d._objectTable.size(), plist.begin());
    }
    
    void PlistBinaryWriter::writePlistBinary(const char *filename, const Plist::Object &message)
    {
        std::ofstream stream(filename, std::ios::binary);
        writePlistBinary(stream, message);
        stream.close();
    }
    
    bool PlistBinaryWriter::HasChinese(const char *str)
    {
        bool nRet = false;
        char c;
        while((c=*str++))
        {
            //If the character height is 1 and the next character height is 1, there is a Chinese character
            if( (c&0x80) && (*str & 0x80) )
            {
                nRet = true;
                break;
            }
        }
        return nRet;
    }
    
#if defined(_MSC_VER)
    void PlistBinaryWriter::writePlistBinary(const wchar_t* filename, const Object& message)
    {
        std::ofstream stream(filename, std::ios::binary);
        writePlistBinary(stream, message);
        stream.close();
    }
#endif
}
