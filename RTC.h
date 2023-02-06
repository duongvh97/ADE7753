#ifndef RTC_DS1307_H
#define RTC_DS1307_H

#include "RTClib.h"

class RtcDs1307
{
private:
    RTC_DS1307 m_rtc;
public:
    static RtcDs1307* getInstance()
    {
        static RtcDs1307 obj;
        return &obj;
    }
    void initRTC();
    void getCurrentTime(int &h, int &m, int &s);
};

#endif