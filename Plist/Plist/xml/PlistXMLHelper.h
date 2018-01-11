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

#include "../data/PlistData.h"
#include "../pugixml/pugixml.hpp"
#include "../utils/PlistUtils.h"

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
            newNode.append_child(pugi::node_pcdata).set_value(PlistUtils::stringFromValue(obj_cast<T>(obj)).c_str());
        }
    };
}

#endif /* PlistXMLHelper_h */
