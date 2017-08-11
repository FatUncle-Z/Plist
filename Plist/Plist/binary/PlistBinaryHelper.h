//
//  PlistBinaryHelper.h
//  Plist
//
//  Created by zhaojun on 2017/8/11.
//  Copyright © 2017年 zhaojun. All rights reserved.
//

#ifndef PlistBinaryHelper_h
#define PlistBinaryHelper_h

#include <vector>

#include "PlistData.h"

namespace Plist
{
    class PlistBinaryHelper
    {
    public:
        template <typename IntegerType>
        inline static IntegerType bytesToInt(const unsigned char* bytes, bool littleEndian)
        {
            IntegerType result = 0;
            if (littleEndian)
                for (int n = sizeof( result ) - 1; n >= 0; n--)
                    result = (result << 8) + bytes[n];
            else
                for (unsigned n = 0; n < sizeof( result ); n++)
                    result = (result << 8) + bytes[n];
            return result;
        }
        
        template<typename IntegerType>
        inline static std::vector<unsigned char> intToBytes(IntegerType val, bool littleEndian)
        {
            unsigned int numBytes = sizeof(val);
            std::vector<unsigned char> bytes(numBytes);
            
            for(unsigned n = 0; n < numBytes; ++n)
                if(littleEndian)
                    bytes[n] = (val >> 8 * n) & 0xff;
                else
                    bytes[numBytes - 1 - n] = (val >> 8 * n) & 0xff;
            
            return bytes;
        }
        
        template<typename T>
        inline static T* vecData(std::vector<T>& vec)
        {
            return (vec.size() > 0) ? &vec[0] : 0;
            // if(vec.size() > 0)
            //		return &vec[0];
            // else
            //		throw Error("vecData trying to get pointer to empty std::vector");
        }
        
        template<typename T>
        const inline static  T* vecData(const std::vector<T>& vec)
        {
            return (vec.size() > 0) ? &vec[0] : 0;
            // if(vec.size() > 0)
            //		return &vec[0];
            // else
            //		throw Error("vecData trying to get pointer to empty std::vector");
        }
        
        inline static bool hostLittleEndian()
        {
            union { uint32_t x; uint8_t c[4]; } u;
            u.x = 0xab0000cd;
            return u.c[0] == 0xcd;
        }
        
        inline static uint32_t nextpow2(uint32_t x)
        {
            --x;
            x |= x >> 1;
            x |= x >> 2;
            x |= x >> 4;
            x |= x >> 8;
            x |= x >> 16;
            return x + 1;
        }
        
        inline static uint32_t ilog2(uint32_t x)
        {
            uint32_t r = 0;
            while (x >>= 1)
                ++r;
            return r;
        }
        
        
        static double bytesToDouble(const unsigned char* bytes, bool littleEndian);
        static std::vector<unsigned char> doubleToBytes(double val, bool littleEndian);
        static std::vector<unsigned char> getRange(const unsigned char* origBytes, int64_t index, int64_t size);
        static std::vector<unsigned char> getRange(const std::vector<unsigned char>& origBytes, int64_t index, int64_t size);
        static std::vector<char> getRange(const char* origBytes, int64_t index, int64_t size);
        static std::vector<unsigned char> regulateNullBytes(const std::vector<unsigned char>& origBytes, unsigned int minBytes);
        
        static int32_t getCount(const PlistHelperData& d, int bytePosition, unsigned char headerByte, int& startOffset);
    };
}

#endif /* PlistBinaryHelper_hpp */
