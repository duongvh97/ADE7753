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

void RtcDs1307::getCurrentTime(int &h, int &m, int &s) {
  DateTime now = m_rtc.now();
  h = now.hour();
  m = now.minute();
  s = now.second();
}
