//
//  PlistXMLReader.h
//  Plist
//
//  Created by zhaojun on 2017/8/14.
//  Copyright © 2017年 zhaojun. All rights reserved.
//

#ifndef PlistXMLReader_h
#define PlistXMLReader_h

#include <iostream>
#include <fstream>

#include "../data/PlistData.h"
#include "../pugixml/pugixml.hpp"

namespace Plist
{
    class PlistXMLReader
    {
    private:
        Dictionary parseDictionary(pugi::xml_node& node);
        Array parseArray(pugi::xml_node& node);
        Date parseDate(pugi::xml_node& node);
        Plist::Object parse(pugi::xml_node& doc);
        
    public:
        void parse(const unsigned char* byteArray, size_t size, Plist::Object &message);
    };
}

#endif /* PlistXMLReader_h */
