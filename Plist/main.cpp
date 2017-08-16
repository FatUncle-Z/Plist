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
    dict["ch"] = std::string("dsdfsfs");
    Plist::PlistXMLWriter writer;
    std::string fileName = path;
    writer.writePlistXML(fileName.append("new.plist").c_str(), dict);
     std::string fileName2 = path;
    Plist::PlistReader reader;
    Plist::Dictionary dt;
    reader.readPlist(fileName2.append("newbinary.plist").c_str(), dt);
    std::string fileName3 = path;
    Plist::PlistBinaryWriter binWriter;
    binWriter.writePlistBinary(fileName3.append("newbinary1.plist").c_str(), dict);
//    // insert code here...
    std::cout << "Hello, World!\n"<<std::endl;
    
    printf("The current directory is: %s\n", path1.c_str());
    return 0;
}
