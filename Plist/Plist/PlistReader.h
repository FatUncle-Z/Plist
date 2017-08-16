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

#include "data/PlistObject.h"
#include "data/PlistData.h"

namespace Plist
{
    class PlistReader
    {
    public:
        void readPlist(const char* byteArrayTemp, int64_t size, Object& message);
        void readPlist(std::istream& stream, Object& message);
        
        
    public:
        template <typename T>
        void readPlist(const char* filename, T& message)
        {
            std::ifstream stream(filename, std::ios::binary);
            if(!stream)
                throw Error("Can't open file.");
            readPlist(stream, message);
        }
        
        template <typename T>
        void readPlist(const char* byteArrayTemp, int64_t size, T& message)
        {
            Plist::Object tmp_message;
            readPlist(byteArrayTemp, size, tmp_message);
            message = obj_cast<T>(tmp_message);
        }
        
        template <typename T>
        void readPlist(std::istream& stream, T& message)
        {
            Plist::Object tmp_message;
            readPlist(stream, tmp_message);
            message = obj_cast<T>(tmp_message);
        }
        
#if defined(_MSC_VER)
        template <typename T>
        void Plist::readPlist(const wchar_t* filename, T& message)
        {
            std::ifstream stream(filename, std::ios::binary);
            if(!stream)
                throw Error("Can't open file.");
            readPlist(stream, message);
        }
#endif
    };
}
#endif /* PlistReader_h */
