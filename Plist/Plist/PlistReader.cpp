//
//  PlistReader.cpp
//  Plist
//
//  Created by zhaojun on 2017/8/11.
//  Copyright © 2017年 zhaojun. All rights reserved.
//

#include "PlistReader.h"
#include "binary/PlistBinaryHelper.h"
#include "pugixml/pugixml.hpp"
#include "xml/PlistXMLReader.h"
#include "binary/PlistBinaryReader.h"

namespace Plist
{
    void PlistReader::readPlist(std::istream &stream, Plist::Object &message)
    {
        long start = stream.tellg();
        stream.seekg(0, std::ifstream::end);
        long size = ((int) stream.tellg()) - start;
        if(size > 0)
        {
            stream.seekg(0, std::ifstream::beg);
            std::vector<char> buffer(size);
            stream.read( (char *)&buffer[0], size );
            
            readPlist(&buffer[0], size, message);
        }
        else
        {
            throw Error("Can't read zero length data");
        }
    }
    
    void PlistReader::readPlist(const char *byteArrayTemp, int64_t size, Plist::Object &message)
    {
        using namespace std;
        const unsigned char* byteArray = (const unsigned char*) byteArrayTemp;
        if (!byteArray || (size == 0))
            throw Error("Plist: Empty plist data");
        
        // infer plist type from header.  If it has the bplist00 header as first 8
        // bytes, then it's a binary plist.  Otherwise, assume it's XML
        
        std::string magicHeader((const char*) byteArray, 8);
        if(magicHeader == "bplist00")
        {
            PlistBinaryReader reader;
            reader.parse(byteArray, size, message);
        }
        else
        {
            PlistXMLReader reader;
            reader.parse(byteArray, (size_t)size, message);
        }
    }

}
