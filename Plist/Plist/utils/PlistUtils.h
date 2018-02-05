//
//  PlistUtils.h
//  Plist
//
//  Created by zhaojun on 2018/1/11.
//  Copyright © 2018年 zhaojun. All rights reserved.
//

#ifndef PlistUtils_h
#define PlistUtils_h
#include <sstream>
#include <codecvt>
namespace Plist
{
    class PlistUtils {
    public:
        
        template<typename T>
        inline static std::string stringFromValue(const T& value)
        {
            std::stringstream ss;
            ss<<value;
            return ss.str();
        }
        
        inline static bool WCharStringToUTF8String(const std::wstring &wstr,std::string &u8str)
        {
            std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
            u8str= conv.to_bytes(wstr);
            return true;
        }
        
        inline static bool UTF8StringToWCharString(const std::string &u8str,std::wstring &wstr)
        {
            std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
            wstr=conv.from_bytes( u8str );
            return true;
        }
    };
}

#endif /* PlistUtils_h */
