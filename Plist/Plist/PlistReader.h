//
//  PlistReader.h
//  Plist
//
//  Created by zhaojun on 2017/8/11.
//  Copyright © 2017年 zhaojun. All rights reserved.
//

#ifndef PlistReader_h
#define PlistReader_h

#include <iostream>
#include <fstream>

#include "PlistObject.h"

namespace Plist
{
    class PlistReader
    {
        void readPlist(const char* byteArrayTemp, int64_t size, Object& message);
        void readPlist(std::istream& stream, Object& message);
        
        template<typename T>
        void readPlist(const char* byteArray, int64_t size, T& message);
        
        template<typename T>
        void readPlist(std::istream& stream, T& message);
        
        template<typename T>
        void readPlist(const char* filename, T& message);
#if defined(_MSC_VER)
        template<typename T>
        void readPlist(const wchar_t* filename, T& message);
#endif
    };
}
#endif /* PlistReader_h */
