//
//  PlistXMLWriter.cpp
//  Plist
//
//  Created by zhaojun on 2017/8/11.
//  Copyright © 2017年 zhaojun. All rights reserved.
//

#include "PlistXMLWriter.h"
#include "base64.hpp"
#include "PlistXMLHelper.h"
#include "../base64/Base64Helper.h"

namespace Plist
{
    void PlistXMLWriter::writePlistXML(Data &plist, const Plist::Object &message)
    {
        std::stringstream ss;
        writePlistXML(ss, message);
        
        std::istreambuf_iterator<char> beg(ss);
        std::istreambuf_iterator<char> end;
        plist.clear();
        plist.insert(plist.begin(), beg, end);
    }
    
    void PlistXMLWriter::writePlistXML(const char *filename, const Plist::Object &message)
    {
        std::ofstream stream(filename, std::ios::binary);
        writePlistXML(stream, message);
        stream.close();
    }
    
#if defined(_MSC_VER)
    void PlistXMLWriter::writePlistXML( const wchar_t* filename, const Object& message)
    {
        std::ofstream stream(filename, std::ios::binary);
        writePlistXML(stream, message);
        stream.close();
    }
#endif
    
    void PlistXMLWriter::writePlistXML(std::ostream &stream, const Plist::Object &message)
    {
        pugi::xml_document doc;
        writePlistXML(doc, message);
        doc.save(stream);
    }
    
    void PlistXMLWriter::writePlistXML(pugi::xml_document &doc, const Plist::Object &message)
    {
        // declaration node
        pugi::xml_node decNode = doc.append_child(pugi::node_declaration);
        decNode.append_attribute("version") = "1.0";
        decNode.append_attribute("encoding") = "UTF-8";
        
        // doctype node
        doc.append_child(pugi::node_doctype).set_value("plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\"");
        
        // root node
        pugi::xml_node plistNode = doc.append_child("plist");
        plistNode.append_attribute("version") = "1.0";
        
        writeXMLNode(plistNode, message);
    }
    
    
    void PlistXMLWriter::writeXMLNode(pugi::xml_node &node, const Plist::Object &obj)
    {
        const std::type_info &objType = obj.getType();
        
        if(objType == typeid(int32_t))
            PlistXMLHelper::writeXMLSimpleNode<int32_t>(node, "integer", obj);
        else if(objType == typeid(int64_t))
            PlistXMLHelper::writeXMLSimpleNode<int64_t>(node, "integer", obj);
        else if(objType == typeid(long))
            PlistXMLHelper::writeXMLSimpleNode<long>(node, "integer", obj);
        else if(objType == typeid(short))
            PlistXMLHelper::writeXMLSimpleNode<short>(node, "integer", obj);
        else if(objType == typeid(Dictionary))
            writeXMLDictionary(node, obj_cast<const Dictionary&>(obj));
        else if(objType == typeid(String))
            PlistXMLHelper::writeXMLSimpleNode<String>(node, "string", obj);
        else if(objType == typeid(Array))
            writeXMLArray(node, obj_cast<const Array&>(obj));
        else if(objType == typeid(Data))
        {
            std::string dataEncoded ;
            Base64Helper::base64Encode(dataEncoded, obj_cast<const Data&>(obj));
            PlistXMLHelper::writeXMLSimpleNode<std::string>(node, "data", dataEncoded);
        }
        else if(objType == typeid(double))
            PlistXMLHelper::writeXMLSimpleNode<double>(node, "real", obj);
        else if(objType == typeid(float))
            PlistXMLHelper::writeXMLSimpleNode<float>(node, "real", obj);
        else if(objType == typeid(Date))
            PlistXMLHelper::writeXMLSimpleNode<std::string>(node, "date", obj_cast<const Date&>(obj).timeAsXMLConvention());
        else if(objType == typeid(bool))
        {
            bool value = obj_cast<const bool&>(obj);
            node.append_child(value ? "true" : "false");
        }
        else {
            PlistXMLHelper::writeXMLSimpleNode<String>(node, "string", obj);
        }
    }
    
    void PlistXMLWriter::writeXMLDictionary(pugi::xml_node &node, const Dictionary &message)
    {
        pugi::xml_node newNode = node.append_child("dict");
        for(Dictionary::const_iterator it = message.begin(); it != message.end(); ++it)
        {
            pugi::xml_node keyNode = newNode.append_child("key");
            keyNode.append_child(pugi::node_pcdata).set_value(it->first.c_str());
            writeXMLNode(newNode, it->second);
        }
    }
    
    void PlistXMLWriter::writeXMLArray(pugi::xml_node &node, const Array &array)
    {
        pugi::xml_node newNode = node.append_child("array");
        for(Array::const_iterator it = array.begin(); it != array.end(); ++it)
            writeXMLNode(newNode, *it);
    }
}
