//
//  PlistDate.h
//  Plist
//
//  Created by zhaojun on 2017/8/11.
//  Copyright © 2017年 zhaojun. All rights reserved.
//

#ifndef PlistDate_h
#define PlistDate_h

#include <ctime>
#include <string>
#include <iostream>

namespace Plist
{
    class Date
    {
    private:
        
        // stored as unix epoch, number of seconds since 01-01-1970
        time_t _time;
        
    public:
        Date();
        
        Date(int month, int day, int year, int hour24, int minute, int second, bool UTC);
        
        void set(int month, int day, int year, int hour24, int minute, int second, bool UTC);
        
        void setToCurrentTime();
        
        time_t secondsSinceDate(const Date& startDate) const;
        // returns -1 : first < second
        //          0 : first = second
        //          1 : first > second
        
        static int compare(const Date& first, const Date& second);
        
        bool operator > (const Date& rhs) const;
        
        bool operator < (const Date& rhs) const;
        
        bool operator == (const Date& rhs) const;
        
        // iso 8601 date string convention
        std::string timeAsXMLConvention() const;
        
        // iso 8601 date string convention
        void setTimeFromXMLConvention(const std::string& timeString);
        
        // Apple epoch is # of seconds since  01-01-2001. So we need to add the
        // number of seconds since 01-01-1970 which is proper unix epoch
        
        void setTimeFromAppleEpoch(double appleTime);
        
        time_t timeAsEpoch() const;
        
        // We need to subtract the number of seconds between 01-01-2001 and
        // 01-01-1970 to get Apple epoch from unix epoch
        
        double timeAsAppleEpoch() const;
    };
}

#endif /* PlistDate_h */
