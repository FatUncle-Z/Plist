//
//  Base64Helper.h
//  Plist
//
//  Created by zhaojun on 2017/8/11.
//  Copyright © 2017年 zhaojun. All rights reserved.
//

#ifndef Base64Helper_h
#define Base64Helper_h

#include "PlistData.h"

namespace Plist
{
    class Base64Helper
    {
    public:
        static void base64Encode(std::string& outString, Data inData);
        static std::vector<char> base64Decode(const char * dataEncoded);
    };
}

#endif /* Base64Helper_h */
