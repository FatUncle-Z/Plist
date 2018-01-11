//
//  PlistUtils.h
//  Plist
//
//  Created by zhaojun on 2018/1/11.
//  Copyright © 2018年 zhaojun. All rights reserved.
//

#ifndef PlistUtils_h
#define PlistUtils_h
#include <sstream>
namespace Plist
{
    class PlistUtils {
    public:
        template<typename T>
        inline static std::string stringFromValue(const T& value)
        {
            std::stringstream ss;
            ss<<value;
            return ss.str();
        }
    };
}

#endif /* PlistUtils_h */
