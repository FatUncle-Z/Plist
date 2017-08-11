//
//  PlistData.h
//  Plist
//
//  Created by zhaojun on 2017/8/11.
//  Copyright © 2017年 zhaojun. All rights reserved.
//

#ifndef PlistData_h
#define PlistData_h
#include <string>
#include <map>
#include <vector>
#include <stdexcept>

#include "PlistObject.h"
#include "PlistDate.h"

namespace Plist
{
    typedef  std::string String;
    typedef int64_t Integer;
    typedef double Real;
    typedef bool Boolean;
    
    typedef std::vector<Object> Array;
    typedef std::map<std::string, Object> Dictionary;
    typedef std::vector<char> Data;
    
    
    //for binary data
    struct PlistHelperData
    {
    public:
        // binary helper data
        std::vector<int32_t> _offsetTable;
        std::vector<unsigned char> _objectTable;
        int32_t _offsetByteSize;
        int64_t _offsetTableOffset;
        
        int32_t _objRefSize;
        int32_t _refCount;
    };
    
    class Error: public std::runtime_error {
    public:
#if __cplusplus >= 201103L
        using std::runtime_error::runtime_error;
#else
        inline Error(const std::string& what)
        : runtime_error(what) { }
#endif
    };
}

#endif /* PlistData_h */
