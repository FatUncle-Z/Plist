//
//  PlistBinaryHelper.cpp
//  Plist
//
//  Created by zhaojun on 2017/8/11.
//  Copyright © 2017年 zhaojun. All rights reserved.
//

#include "PlistBinaryHelper.h"

namespace Plist
{
    double PlistBinaryHelper::bytesToDouble(const unsigned char *bytes, bool littleEndian)
    {
        double result;
        int numBytes = sizeof(double);
        if(littleEndian)
            memcpy( &result, bytes, numBytes);
        else
        {
            std::vector<unsigned char> bytesReverse(numBytes);
            std::reverse_copy(bytes, bytes + numBytes, bytesReverse.begin());
            memcpy(&result, vecData(bytesReverse), numBytes);
        }
        return result;
    }
    
    std::vector<unsigned char> PlistBinaryHelper::doubleToBytes(double val, bool littleEndian)
    {
        std::vector<unsigned char> result(sizeof(double));
        memcpy(vecData(result), &val, sizeof(double));
        if(!littleEndian)
            std::reverse(result.begin(), result.end());
        
        return result;
    }
    
    std::vector<unsigned char> PlistBinaryHelper::getRange(const unsigned char *origBytes, int64_t index, int64_t size)
    {
        std::vector<unsigned char> result((std::vector<unsigned char>::size_type)size);
        std::copy(origBytes + index, origBytes + index + size, result.begin());
        return result;
    }
    
    std::vector<unsigned char> PlistBinaryHelper::getRange(const std::vector<unsigned char> &origBytes, int64_t index, int64_t size)
    {
        if((index + size) > (int64_t) origBytes.size())
            throw Error("Out of bounds getRange");
        return getRange(vecData(origBytes), index, size);
    }
    
    std::vector<char> PlistBinaryHelper::getRange(const char *origBytes, int64_t index, int64_t size)
    {
        std::vector<char> result((std::vector<char>::size_type)size);
        std::copy(origBytes + index, origBytes + index + size, result.begin());
        return result;
    }
    
    std::vector<unsigned char>PlistBinaryHelper::regulateNullBytes(const std::vector<unsigned char> &origBytes, unsigned int minBytes)
    {
        std::vector<unsigned char> bytes(origBytes);
        while((bytes.back() == 0) && (bytes.size() > minBytes))
            bytes.pop_back();
        
        while(bytes.size() < minBytes)
            bytes.push_back(0);
        
        return bytes;
    }
    
    int32_t PlistBinaryHelper::getCount(const PlistHelperData &d, int bytePosition, unsigned char headerByte, int &startOffset)
    {
//        unsigned char headerByteTrail = headerByte & 0xf;
//        if (headerByteTrail < 15)
//        {
//            startOffset = 1;
//            return headerByteTrail;
//        }
//        else
//        {
//            int32_t count = (int32_t)parseBinaryInt(d, bytePosition + 1, startOffset);
//            startOffset += 2;
//            return count;
//        }
        return 1;
    }
}
