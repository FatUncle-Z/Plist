//
//  PlistXMLReader.cpp
//  Plist
//
//  Created by zhaojun on 2017/8/14.
//  Copyright © 2017年 zhaojun. All rights reserved.
//

#include "PlistXMLReader.h"
#include "../base64/Base64Helper.h"

namespace Plist {
    Dictionary PlistXMLReader::parseDictionary(pugi::xml_node &node)
    {
        Dictionary dict;
        for(pugi::xml_node_iterator it = node.begin(); it != node.end(); ++it)
        {
            if(std::string("key") != it->name())
                throw Error("Plist: XML dictionary key expected but not found");
            
            std::string key(it->first_child().value());
            ++it;
            
            if(it == node.end())
                throw Error("Plist: XML dictionary value expected for key " + key + "but not found");
            else if(std::string("key") == it->name())
                throw Error("Plist: XML dictionary value expected for key " + key + "but found another key node");
            
            dict[key] = parse(*it);
        }
        return dict;
    }
    
    Array PlistXMLReader::parseArray(pugi::xml_node &node)
    {
        Array array;
        for(pugi::xml_node_iterator it = node.begin(); it != node.end(); ++it)
            array.push_back(parse(*it));
        return array;
    }
    
    Date PlistXMLReader::parseDate(pugi::xml_node &node)
    {
        Date date;
        date.setTimeFromXMLConvention(node.first_child().value());
        return date;
    }
    
    Object PlistXMLReader::parse(pugi::xml_node &node)
    {
        std::string nodeName = node.name();
        
        Object result;
        if("dict" == nodeName)
            result = parseDictionary(node);
        else if("array" == nodeName)
            result = parseArray(node);
        else if("string" == nodeName)
            result = std::string(node.first_child().value());
        else if("integer" == nodeName)
            result = (int64_t) atoll(node.first_child().value());
        else if("real" == nodeName)
            result = atof(node.first_child().value());
        else if("false" == nodeName)
            result = bool(false);
        else if("true" == nodeName)
            result = bool(true);
        else if("data" == nodeName)
            result = Base64Helper::base64Decode(node.first_child().value());
        else if("date" == nodeName)
            result = parseDate(node);
        else
            throw Error(std::string("Plist: XML unknown node type " + nodeName));
        
        return result;
    }
    
    void PlistXMLReader::parse(const unsigned char *byteArray, size_t size, Plist::Object &message)
    {
        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_buffer(byteArray, (size_t)size);
        if(!result)
            throw Error((std::string("Plist: XML parsed with error ") + result.description()).c_str());
        
        pugi::xml_node rootNode = doc.child("plist").first_child();
        message = parse(rootNode);
    }
}
