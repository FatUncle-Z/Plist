//
//  Base64Helper.cpp
//  Plist
//
//  Created by zhaojun on 2017/8/11.
//  Copyright © 2017年 zhaojun. All rights reserved.
//

#include "Base64Helper.h"
#include "base64.hpp"

namespace Plist
{
    void Base64Helper::base64Encode(std::string &outString, Data inData)
    {
        outString.clear();
        std::insert_iterator<std::string> ii(outString, outString.begin());
        base64<char> b64;
        int state = 0;
        
#if defined(_WIN32) || defined(_WIN64)
        b64.put(inData.begin(), inData.end(), ii, state , base64<>::crlf());
#else
        b64.put(inData.begin(), inData.end(), ii, state , base64<>::lf());
#endif
    }
    
    std::vector<char> Base64Helper::base64Decode(const char *encodedData)
    {
        std::vector<char> data;
        std::insert_iterator<std::vector<char> > ii(data, data.begin());
        base64<char> b64;
        int state = 0;
        b64.get(encodedData, encodedData + strlen(encodedData), ii, state);
        return data;
    }
}
