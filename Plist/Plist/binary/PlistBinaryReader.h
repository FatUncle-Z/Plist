//
//  PlistBinaryReader.h
//  Plist
//
//  Created by zhaojun on 2017/8/14.
//  Copyright © 2017年 zhaojun. All rights reserved.
//

#ifndef PlistBinaryReader_h
#define PlistBinaryReader_h
#include <codecvt>

#include "../data/PlistData.h"

namespace Plist
{
    class PlistBinaryReader
    {
    private:
#if _MSC_VER && _MSC_VER >= 1900
        std::wstring_convert<std::codecvt_utf8<int16_t>, int16_t> converter;
#else
        std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> converter;
#endif
        
    private:
        Object parseBinary(const PlistHelperData& d, int objRef);
        Dictionary  parseBinaryDictionary(const PlistHelperData& d, int objRef);
        Array  parseBinaryArray(const PlistHelperData& d, int objRef);
        int64_t parseBinaryInt(const PlistHelperData& d, int headerPosition, int& intByteCount);
        double parseBinaryReal(const PlistHelperData& d, int headerPosition);
        Date parseBinaryDate(const PlistHelperData& d, int headerPosition);
        bool parseBinaryBool(const PlistHelperData& d, int headerPosition);
        std::string parseBinaryString(const PlistHelperData& d, int headerPosition);
        std::string parseBinaryUnicode(const PlistHelperData& d, int headerPosition);
        Data parseBinaryByteArray(const PlistHelperData& d, int headerPosition);
        
        
        void parseTrailer(PlistHelperData& d, const std::vector<unsigned char>& trailer);
        void parseOffsetTable(PlistHelperData& d, const std::vector<unsigned char>& offsetTableBytes);
        
        int32_t getCount(const PlistHelperData& d, int bytePosition, unsigned char headerByte, int& startOffset);
        std::vector<int32_t> getRefsForContainers(const PlistHelperData& d, int objRef);
        
    public:
        void parse(const unsigned char *byteArray, size_t size, Plist::Object &message);
    };
}

#endif /* PlistBinaryReader_h */
