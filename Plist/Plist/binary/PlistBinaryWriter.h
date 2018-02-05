//
//  PlistBinaryWriter.h
//  Plist
//
//  Created by zhaojun on 2017/8/11.
//  Copyright © 2017年 zhaojun. All rights reserved.
//

#ifndef PlistBinaryWriter_h
#define PlistBinaryWriter_h

#include "../data/PlistData.h"

namespace Plist
{
    /*****************************************************************************/
    /********************      PlistBinary               *************************/
    /*****************************************************************************/
    class PlistBinaryWriter {
        
    private:
        void writePlistBinary(PlistHelperData& d, const Object& message);
        int countAny(const Object& object);
        int countDictionary(const Dictionary& dictionary);
        int countArray(const Array& array);
        
        std::vector<unsigned char> writeBinaryDictionary(PlistHelperData& d, const Dictionary& dictionary);
        std::vector<unsigned char> writeBinaryArray(PlistHelperData& d, const Array& array);
        std::vector<unsigned char> writeBinaryByteArray(PlistHelperData& d, const Data& byteArray);
        std::vector<unsigned char> writeBinaryInteger(PlistHelperData& d, int64_t value, bool write);
        std::vector<unsigned char> writeBinaryBool(PlistHelperData& d, bool value);
        std::vector<unsigned char> writeBinaryDate(PlistHelperData& d, const Date& date);
        std::vector<unsigned char> writeBinaryDouble(PlistHelperData& d, double value);
        std::vector<unsigned char> writeBinary(PlistHelperData& d, const Object& obj);
        std::vector<unsigned char> writeBinaryString(PlistHelperData& d, const std::string& value, bool head);
        
        std::vector<unsigned char> writeBinaryUTF8String(PlistHelperData& d, const std::string& value, bool head);
        
        std::vector<unsigned char> writeBinaryUnicodeString(PlistHelperData& d, const std::string& value, bool head);
        std::vector<unsigned char> writeBinaryUnicodeWString(Plist::PlistHelperData &d, const std::wstring &value, bool head);

        //Return false: no Chinese, return true: there is Chinese
        bool HasChinese(const char *str);
    public:
        void writePlistBinary(std::ostream& stream, const Object& message);
        void writePlistBinary(Data& plist, const Object& message);
        void writePlistBinary(const char* filename, const Object& message);
#if defined(_MSC_VER)
        void writePlistBinary(const wchar_t* filename, const Object& message);
#endif
        char *C2W(wchar_t w_cn , char c_cn[2])
        {
            //following code convert wchar to char
            c_cn[0] = w_cn >> 8 ;
            c_cn[1] = w_cn ;
            c_cn[2] = '\0';
            return c_cn ;
        }
    };
}

#endif /* PlistBinaryWriter_h */
