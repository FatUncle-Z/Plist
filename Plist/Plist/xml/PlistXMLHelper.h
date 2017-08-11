//
//  PlistXMLHelper.h
//  Plist
//
//  Created by zhaojun on 2017/8/11.
//  Copyright © 2017年 zhaojun. All rights reserved.
//

#ifndef PlistXMLHelper_h
#define PlistXMLHelper_h

#include <sstream>

#include "PlistData.h"
#include "../pugixml/pugixml.hpp"

namespace Plist
{
    class PlistXMLHelper {
        
    //template function
    public:
        template<typename T>
        inline static void writeXMLSimpleNode(pugi::xml_node& node, const char* name, const Object& obj)
        {
            pugi::xml_node newNode;
            newNode = node.append_child(name);
            newNode.append_child(pugi::node_pcdata).set_value(stringFromValue(obj).c_str());
        }
        
        template<typename T>
        inline static std::string stringFromValue(const T& value)
        {
            std::stringstream ss;
            ss<<value;
            return ss.str();
        }
    };
}

#endif /* PlistXMLHelper_h */
