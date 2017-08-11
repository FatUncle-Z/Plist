//
//  PlistXMLWriter.h
//  Plist
//
//  Created by zhaojun on 2017/8/11.
//  Copyright © 2017年 zhaojun. All rights reserved.
//

#ifndef PlistXMLWriter_h
#define PlistXMLWriter_h

#include <iostream>
#include <fstream>


#include "../PlistData.h"
#include "../pugixml/pugixml.hpp"

namespace Plist
{
    /*****************************************************************************/
    /********************      Plist XML                 *************************/
    /*****************************************************************************/
    class PlistXMLWriter
    {
    private:
        void writePlistXML(pugi::xml_document& doc, const Object& message);
        void writeXMLArray(pugi::xml_node& node, const Array& array);
        void writeXMLDictionary(pugi::xml_node& node, const Dictionary& message);
        void writeXMLNode(pugi::xml_node& node, const Object& obj);
        
    public:
        // Public XML write methods.
        void writePlistXML(std::ostream& stream, const Object& message);
        void writePlistXML(Data& plist, const Object& message);
        void writePlistXML(const char* filename, const Object& message);
#if defined(_MSC_VER)
        void writePlistXML(const wchar_t* filename, const Object& message);
#endif

    };
}

#endif /* PlistXMLWriter_h */
