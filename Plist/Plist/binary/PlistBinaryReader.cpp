//
//  PlistBinaryReader.cpp
//  Plist
//
//  Created by zhaojun on 2017/8/14.
//  Copyright © 2017年 zhaojun. All rights reserved.
//
#include <sstream>
#include <codecvt>

#include "PlistBinaryReader.h"
#include "PlistBinaryHelper.h"

namespace Plist
{
    Object PlistBinaryReader::parseBinary(const Plist::PlistHelperData &d, int objRef)
    {
        unsigned char header = d._objectTable[d._offsetTable[objRef]];
        switch (header & 0xF0)
        {
            case 0x00:
            {
                return parseBinaryBool(d, d._offsetTable[objRef]);
            }
            case 0x10:
            {
                int intByteCount;
                return parseBinaryInt(d, d._offsetTable[objRef], intByteCount);
            }
            case 0x20:
            {
                return parseBinaryReal(d, d._offsetTable[objRef]);
            }
            case 0x30:
            {
                return parseBinaryDate(d, d._offsetTable[objRef]);
            }
            case 0x40:
            {
                return parseBinaryByteArray(d, d._offsetTable[objRef]);
            }
            case 0x50:
            {
                return parseBinaryString(d, d._offsetTable[objRef]);
            }
            case 0x60:
            {
                return parseBinaryUnicode(d, d._offsetTable[objRef]);
            }
            case 0xD0:
            {
                return parseBinaryDictionary(d, objRef);
            }
            case 0xA0:
            {
                return parseBinaryArray(d, objRef);
            }
        }
        throw Error("This type is not supported");
    }
    
    Dictionary PlistBinaryReader::parseBinaryDictionary(const Plist::PlistHelperData &d, int objRef)
    {
        std::vector<int32_t> refs = getRefsForContainers(d, objRef);
        unsigned long refCount = refs.size() / 2;
        
        Dictionary dict;
        for (int i = 0; i < refCount; i++)
        {
            Object keyAny = parseBinary(d, refs[i]);
            
            try
            {
                std::string& key = obj_cast<std::string&>(keyAny);
                dict[key] =  parseBinary(d, refs[i + refCount]);
            }
            catch(bad_obj_cast& )
            {
                throw Error("Error parsing dictionary.  Key can't be parsed as a string");
            }
        }
        
        return dict;
    }
    
    Array PlistBinaryReader::parseBinaryArray(const Plist::PlistHelperData &d, int objRef)
    {
        std::vector<int32_t> refs = getRefsForContainers(d, objRef);
        unsigned long refCount = refs.size();
        
        Array array;
        for(int i = 0; i < refCount; ++i)
            array.push_back(parseBinary(d, refs[i]));
        
        return array;
    }
    
    int64_t PlistBinaryReader::parseBinaryInt(const Plist::PlistHelperData &d, int headerPosition, int &intByteCount)
    {
        unsigned char header = d._objectTable[headerPosition];
        intByteCount = 1 << (header & 0xf);
        std::vector<unsigned char> buffer = PlistBinaryHelper::getRange(d._objectTable, headerPosition + 1, intByteCount);
        reverse(buffer.begin(), buffer.end());
        
        return PlistBinaryHelper::bytesToInt<int64_t>(PlistBinaryHelper::vecData(PlistBinaryHelper::regulateNullBytes(buffer, 8)), PlistBinaryHelper::hostLittleEndian());
    }
    
    double PlistBinaryReader::parseBinaryReal(const Plist::PlistHelperData &d, int headerPosition)
    {
        unsigned char header = d._objectTable[headerPosition];
        int byteCount = 1 << (header & 0xf);
        std::vector<unsigned char> buffer = PlistBinaryHelper::getRange(d._objectTable, headerPosition + 1, byteCount);
        reverse(buffer.begin(), buffer.end());
        
        return PlistBinaryHelper::bytesToDouble(PlistBinaryHelper::vecData(PlistBinaryHelper::regulateNullBytes(buffer, 8)), PlistBinaryHelper::hostLittleEndian());
    }
    
    Date PlistBinaryReader::parseBinaryDate(const Plist::PlistHelperData &d, int headerPosition)
    {
        // date always an 8 byte float starting after full byte header
        std::vector<unsigned char> buffer = PlistBinaryHelper::getRange(d._objectTable, headerPosition + 1, 8);
        
        Date date;
        
        // Date is stored as Apple Epoch and big endian.
        date.setTimeFromAppleEpoch(PlistBinaryHelper::bytesToDouble(PlistBinaryHelper::vecData(buffer), false));
        
        return date;
    }
    
    bool PlistBinaryReader::parseBinaryBool(const Plist::PlistHelperData &d, int headerPosition)
    {
        unsigned char header = d._objectTable[headerPosition];
        bool value;
        if(header == 0x09)
            value = true;
        else if (header == 0x08)
            value = false;
        else if (header == 0x00)
        {
            // null byte, not sure yet what to do with this.  It's in the spec but we
            // have never encountered it.
            
            throw Error("Plist: null byte encountered, unsure how to parse");
        }
        else if (header == 0x0F)
        {
            // fill byte, not sure yet what to do with this.  It's in the spec but we
            // have never encountered it.
            
            throw Error("Plist: fill byte encountered, unsure how to parse");
        }
        else
        {
            std::stringstream ss;
            ss<<"Plist: unknown header "<<header;
            throw Error(ss.str().c_str());
        }
        
        return value;
    }
    
    std::string PlistBinaryReader::parseBinaryString(const Plist::PlistHelperData &d, int headerPosition)
    {
        unsigned char headerByte = d._objectTable[headerPosition];
        int charStartPosition;
        int32_t charCount = getCount(d, headerPosition, headerByte, charStartPosition);
        charStartPosition += headerPosition;
        
        std::vector<unsigned char> characterBytes = PlistBinaryHelper::getRange(d._objectTable, charStartPosition, charCount);
        std::string buffer = std::string((char*) PlistBinaryHelper::vecData(characterBytes), characterBytes.size());
        return buffer;
    }
    
    std::string PlistBinaryReader::parseBinaryUnicode(const Plist::PlistHelperData &d, int headerPosition)
    {
        unsigned char headerByte = d._objectTable[headerPosition];
        int charStartPosition;
        int32_t charCount = getCount(d, headerPosition, headerByte, charStartPosition);
        charStartPosition += headerPosition;
        
        std::vector<unsigned char> characterBytes = PlistBinaryHelper::getRange(d._objectTable, charStartPosition, charCount * 2);
        
        std::vector<std::vector<unsigned char>> unicodeBytes;
        unicodeBytes.reserve(characterBytes.size() / 2);
        
        if (PlistBinaryHelper::hostLittleEndian()) {
            if (! characterBytes.empty()) {
                for (std::size_t i = 0, n = characterBytes.size(); i < n - 1; i += 2) {
                    std::swap(characterBytes[i], characterBytes[i + 1]);
                    std::vector<unsigned char> unicode;
                    unicode.reserve(2);
                    unicode.emplace_back(characterBytes[i]);
                    unicode.emplace_back(characterBytes[i+1]);
                    unicodeBytes.emplace_back(unicode);
                }
            }
        }
        
        std::string ret;
        std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> converter;
        for (int i = 0; i < unicodeBytes.size(); ++i){
            int16_t *u16chars = (int16_t*) PlistBinaryHelper::vecData(unicodeBytes[i]);
            ret.append(converter.to_bytes(*u16chars).c_str());
        }
        
        return ret;
    }
    
    Data PlistBinaryReader::parseBinaryByteArray(const Plist::PlistHelperData &d, int headerPosition)
    {
        unsigned char headerByte = d._objectTable[headerPosition];
        int byteStartPosition;
        int32_t byteCount = getCount(d, headerPosition, headerByte, byteStartPosition);
        byteStartPosition += headerPosition;
        
        return PlistBinaryHelper::getRange((const char*) PlistBinaryHelper::vecData(d._objectTable), byteStartPosition, byteCount);

    }
    
    void PlistBinaryReader::parseTrailer(Plist::PlistHelperData &d, const std::vector<unsigned char> &trailer)
    {
        d._offsetByteSize = PlistBinaryHelper::bytesToInt<int32_t>(PlistBinaryHelper::vecData(PlistBinaryHelper::regulateNullBytes(PlistBinaryHelper::getRange(trailer, 6, 1), 4)), PlistBinaryHelper::hostLittleEndian());
        d._objRefSize = PlistBinaryHelper::bytesToInt<int32_t>(PlistBinaryHelper::vecData(PlistBinaryHelper::regulateNullBytes(PlistBinaryHelper::getRange(trailer, 7, 1), 4)), PlistBinaryHelper::hostLittleEndian());
        
        std::vector<unsigned char> refCountBytes = PlistBinaryHelper::getRange(trailer, 12, 4);
        //	std::reverse(refCountBytes.begin(), refCountBytes.end());
        d._refCount = PlistBinaryHelper::bytesToInt<int32_t>(PlistBinaryHelper::vecData(refCountBytes), false);
        
        std::vector<unsigned char> offsetTableOffsetBytes = PlistBinaryHelper::getRange(trailer, 24, 8);
        //	std::reverse(offsetTableOffsetBytes.begin(), offsetTableOffsetBytes.end());
        d._offsetTableOffset = PlistBinaryHelper::bytesToInt<int64_t>(PlistBinaryHelper::vecData(offsetTableOffsetBytes), false);
    }
    
    void PlistBinaryReader::parseOffsetTable(Plist::PlistHelperData &d, const std::vector<unsigned char> &offsetTableBytes)
    {
        for (unsigned int i = 0; i < offsetTableBytes.size(); i += d._offsetByteSize)
        {
            std::vector<unsigned char> temp = PlistBinaryHelper::getRange(offsetTableBytes, i, d._offsetByteSize);
            std::reverse(temp.begin(), temp.end());
            d._offsetTable.push_back(
                                     PlistBinaryHelper::bytesToInt<int32_t>(
                                                         PlistBinaryHelper::vecData(PlistBinaryHelper::regulateNullBytes(temp, 4)), PlistBinaryHelper::hostLittleEndian()));
        }
    }
    
    int32_t PlistBinaryReader::getCount(const Plist::PlistHelperData &d, int bytePosition, unsigned char headerByte, int &startOffset)
    {
        unsigned char headerByteTrail = headerByte & 0xf;
        if (headerByteTrail < 15)
        {
            startOffset = 1;
            return headerByteTrail;
        }
        else
        {
            int32_t count = (int32_t)parseBinaryInt(d, bytePosition + 1, startOffset);
            startOffset += 2;
            return count;
        }
    }
    
    std::vector<int32_t> PlistBinaryReader::getRefsForContainers(const Plist::PlistHelperData &d, int objRef)
    {
        int32_t refCount = 0;
        int refStartPosition;
        refCount = getCount(d, d._offsetTable[objRef], d._objectTable[d._offsetTable[objRef]], refStartPosition);
        refStartPosition += d._offsetTable[objRef];
        
        std::vector<int32_t> refs;
        int mult = 1;
        if((((unsigned char) d._objectTable[d._offsetTable[objRef]]) & 0xF0) == 0xD0)
            mult = 2;
        for (int i = refStartPosition; i < refStartPosition + refCount * mult * d._objRefSize; i += d._objRefSize)
        {
            std::vector<unsigned char> refBuffer = PlistBinaryHelper::getRange(d._objectTable, i, d._objRefSize);
            reverse(refBuffer.begin(), refBuffer.end());
            refs.push_back(PlistBinaryHelper::bytesToInt<int32_t>(PlistBinaryHelper::vecData(PlistBinaryHelper::regulateNullBytes(refBuffer, 4)), PlistBinaryHelper::hostLittleEndian()));
        }
        
        return refs;
    }
    
    void PlistBinaryReader::parse(const unsigned char *byteArray, size_t size, Plist::Object &message)
    {
        PlistHelperData d;
        parseTrailer(d, PlistBinaryHelper::getRange(byteArray, size - 32, 32));
        
        d._objectTable = PlistBinaryHelper::getRange(byteArray, 0, d._offsetTableOffset);
        std::vector<unsigned char> offsetTableBytes = PlistBinaryHelper::getRange(byteArray, d._offsetTableOffset, size - d._offsetTableOffset - 32);
        
        parseOffsetTable(d, offsetTableBytes);
        
        message = parseBinary(d, 0);
    }
}
