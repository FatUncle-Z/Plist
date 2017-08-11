//
//  main.cpp
//  Plist
//
//  Created by zhaojun on 2017/8/11.
//  Copyright © 2017年 zhaojun. All rights reserved.
//

#include <dirent.h>
#include <iostream>
#include "PlistData.h"
#include "PlistWriter.h"
#include "PlistXMLWriter.h"
#include "PlistBinaryWriter.h"

int main(int argc, const char * argv[]) {
    Plist::Object obj = 20;
    std::string path = std::string(argv[0]);
    size_t split = path.find_last_of("/");
    std::string path1 = path.substr(0, split);
    Plist::Dictionary dict;
    dict["key1"] = 20;
    dict["tets"] = "dsfsfasf";
    Plist::PlistXMLWriter writer;
    std::string fileName = path;
    writer.writePlistXML(fileName.append("new.plist").c_str(), dict);
     std::string fileName2 = path;
    Plist::PlistBinaryWriter binWriter;
    binWriter.writePlistBinary(fileName2.append("newbinary.plist").c_str(), dict);
    // insert code here...
    std::cout << "Hello, World!\n"<<obj<<std::endl;
    
    printf("The current directory is: %s\n", path1.c_str());
    return 0;
}
