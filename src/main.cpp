// now.pde
// Prints a snapshot of the current date and time along with the UNIX time
// Modified by Andy Wickert from the JeeLabs / Ladyada RTC library examples
// 5/15/11
#include <Arduino.h>
#include <Wire.h>
#include "DS3231.h"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

RTClib RTC;
DS3231 Clock;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

char ssid[] = "Sex Addicts"; //  your network SSID (name)
char pass[] = "Hardass1";

char myTime[] = "1903041090800"; //YYMMDDwHHMMSS

byte Year;
byte Month;
byte Date;
byte DoW;
byte Hour;
byte Minute;
byte Second;

void setTime();
void NTCTimeInit();

void setup()
{
    Serial.begin(57600);
    WiFi.begin(ssid, pass);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    NTCTimeInit();
    setTime();

    Wire.begin();
    Clock.setClockMode(true);
    Clock.setYear(Year);
    Clock.setMonth(Month);
    Clock.setDate(Date);
    Clock.setDoW(DoW);
    Clock.setHour(Hour);
    Clock.setMinute(Minute);
    Clock.setSecond(Second);
}

void loop()
{

    delay(1000);

    volatile DateTime now = RTC.now();

    // Serial.print(now.year(), DEC);
    // Serial.print('/');
    // Serial.print(now.month(), DEC);
    // Serial.print('/');
    // Serial.print(now.day(), DEC);
    // Serial.print(' ');
    // Serial.print(now.hour(), DEC);
    // Serial.print(':');
    // Serial.print(now.minute(), DEC);
    // Serial.print(':');
    // Serial.print(now.second(), DEC);
    // Serial.println();
}

void setTime()
{
    byte Temp1, Temp2;
    // Read Year first
    Temp1 = (byte)myTime[0] - 48;
    Temp2 = (byte)myTime[1] - 48;
    Year = Temp1 * 10 + Temp2;
    // now month
    Temp1 = (byte)myTime[2] - 48;
    Temp2 = (byte)myTime[3] - 48;
    Month = Temp1 * 10 + Temp2;
    // now date
    Temp1 = (byte)myTime[4] - 48;
    Temp2 = (byte)myTime[5] - 48;
    Date = Temp1 * 10 + Temp2;
    // now Day of Week
    DoW = (byte)myTime[6] - 48;
    // now Hour
    Temp1 = (byte)myTime[7] - 48;
    Temp2 = (byte)myTime[8] - 48;
    Hour = Temp1 * 10 + Temp2;
    // now Minute
    Temp1 = (byte)myTime[9] - 48;
    Temp2 = (byte)myTime[10] - 48;
    Minute = Temp1 * 10 + Temp2;
    // now Second
    Temp1 = (byte)myTime[11] - 48;
    Temp2 = (byte)myTime[12] - 48;
    Second = Temp1 * 10 + Temp2;
}

void NTCTimeInit()
{
    timeClient.begin();

    timeClient.update();

    Serial.flush();
    Serial.println();
    Serial.println();
    Serial.println();
    

    Serial.println("time is: ");
    Serial.println(timeClient.getDay());
    Serial.println(timeClient.getFormattedTime());

}