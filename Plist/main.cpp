//
//  main.cpp
//  Plist
//
//  Created by zhaojun on 2017/8/11.
//  Copyright © 2017年 zhaojun. All rights reserved.
//
#include <iostream>
#include "Plist/data/PlistData.h"
#include "Plist/PlistWriter.h"
#include "Plist/xml/PlistXMLWriter.h"
#include "Plist/binary/PlistBinaryWriter.h"
#include "Plist/xml/PlistXMLReader.h"
#include "Plist/PlistReader.h"

int main(int argc, const char * argv[]) {
    std::string path = std::string(argv[0]);
    size_t split = path.find_last_of("/");
    std::string path1 = path.substr(0, split);
    Plist::Dictionary dict;
    dict["key1"] = 20;
    dict["测试"] = 40;
    dict["234L"] = L"测试";
    dict["234"] = "测试";
    std::wstring wstr = L"博客";
    dict["2347"] = wstr;
    dict["ch"] = "dsdfsfs";
    Plist::PlistXMLWriter writer;
    std::string fileName = path;
    writer.writePlistXML(fileName.append("new.plist").c_str(), dict);
     std::string fileName2 = path;
    
    Plist::Dictionary dt;
    
    std::string fileName3 = path;
    Plist::PlistBinaryWriter binWriter;
    binWriter.writePlistBinary(fileName3.append("newbinary1.plist").c_str(), dict);
//    // insert code here...
    std::cout << "Hello, World!\n"<<std::endl;
    
    printf("The current directory is: %s\n", path1.c_str());
    
    Plist::PlistReader reader;
    reader.readPlist(fileName3.c_str(), dt);
    Plist::Object obj = dt["2347"];
    std::string val = "null";
    if (obj.isType<std::string>())
    {
        val = Plist::obj_cast<std::string>(obj);
    }
    std::cout<<val<<std::endl;
    
    wchar_t*  str=L"ABC我们";
    char* s=(char*)str;
    std::cout<<s<<std::endl;
    return 0;
}
