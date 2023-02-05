#include "RTC.h"

void RtcDs1307::initRTC()
{
    if (!m_rtc.begin())
    {
        Serial.println("Couldn't find RTC");
        Serial.flush();
        while (!m_rtc.begin())
        {
            delay(200);
            Serial.println("Couldn't find RTC");
        }
    }
    else
    {
        Serial.println("RTC Ok");
    }

    if (!m_rtc.isrunning())
    {
        Serial.println("RTC is NOT running, let's set the time!");
    }
    else
    {
        Serial.println("RTC is running!");
    }
}