#include "Arduino.h"
#include <SoftwareSerial.h>

// RTC
#include <Wire.h>
#include "RTClib.h" //library file for DS3231 RTC Module

//Finger Print
#include "Adafruit_Fingerprint.h" //fingerprint library header file
#include <EEPROM.h>               //command for storing data

//LCD module
#include <LiquidCrystal.h> //lcd header file
#include <LCDKeypad.h>

//SDCard
#include <SPI.h>
#include <SD.h>

//Function Prototypes

void attendance(int id);
void checkKeys();
void Enroll();
void delet();
uint8_t getFingerprintEnroll();
int getFingerprintIDez();
uint8_t deleteFingerprint(uint8_t id);
void download(int eepIndex);
void SDCardRecord(int i);

//SDcard
const int chipSelect = 53;

//LCD Module
LCDKeypad lcd;
//LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

//Fingerprint
#define fingerPrint Serial1 //for tx/rx communication between arduino & r305 fingerprint sensor
//SoftwareSerial fingerPrint(12,13);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerPrint);

// RTC
RTC_DS3231 rtc;

uint8_t id;

#define indFinger 22 //7
uint8_t buzzer=31;
uint8_t buzzerN = buzzer+6;

#define records 10 // 10 for 10 user

int user1, user2, user3, user4, user5, user6, user7, user8, user9, user10;

DateTime now;

void setup()
{
    delay(1000);
    lcd.begin(16, 2);
    Serial.begin(9600);

    pinMode(10, OUTPUT);
    digitalWrite(10, HIGH);
    pinMode(buzzer, OUTPUT);
    pinMode(buzzerN, OUTPUT);
    pinMode(indFinger, OUTPUT);
    digitalWrite(buzzer, LOW);
    digitalWrite(buzzerN, LOW);

    if (lcd.button() == KEYPAD_SELECT)
    {
        digitalWrite(buzzer, HIGH);
        delay(500);
        digitalWrite(buzzer, LOW);
        lcd.clear();
        lcd.print("Please wait !");
        lcd.setCursor(0, 1);
        lcd.print("Downloding Data");

        Serial.println("Please wait");
        Serial.println("Downloding Data..");
        Serial.println();

        Serial.print("S.No. ");
        for (int i = 0; i < records; i++)
        {
            digitalWrite(buzzer, HIGH);
            delay(500);
            digitalWrite(buzzer, LOW);
            Serial.print(" User ID");
            Serial.print(i + 1);
            Serial.print(" ");
        }
        Serial.println();
        int eepIndex = 0;
        for (int i = 0; i < 30; i++)
        {
            if (i + 1 < 10)
                Serial.print('0');
            Serial.print(i + 1);
            Serial.print(" ");
            eepIndex = (i * 7);
            download(eepIndex);
            eepIndex = (i * 7) + 210;
            download(eepIndex);
            eepIndex = (i * 7) + 420;
            download(eepIndex);
            eepIndex = (i * 7) + 630;
            download(eepIndex);
            eepIndex = (i * 7) + 840;
            download(eepIndex);
            eepIndex = (i * 7) + 1050;
            download(eepIndex);
            eepIndex = (i * 7) + 1260;
            download(eepIndex);
            eepIndex = (i * 7) + 1470;
            download(eepIndex);
            eepIndex = (i * 7) + 1680;
            download(eepIndex);
            Serial.println();
        }
    }
    if (lcd.button() == KEYPAD_LEFT)
    {
        lcd.clear();
        lcd.print("Please Wait");
        lcd.setCursor(0, 1);
        lcd.print("Reseting.....");
        for (int i = 1000; i < 1005; i++)
            EEPROM.write(i, 0);
        for (int i = 0; i < 841; i++)
            EEPROM.write(i, 0xff);
        lcd.clear();
        lcd.print("System Reset");
        delay(1000);
    }

    lcd.clear();
    lcd.print(" Fingerprint ");
    lcd.setCursor(0, 1);
    lcd.print("Attendance System");
    delay(2000);
    lcd.clear();

    digitalWrite(buzzer, HIGH);
    delay(500);
    digitalWrite(buzzer, LOW);
    for (int i = 1000; i < 1000 + records; i++)
    {
        if (EEPROM.read(i) == 0xff)
            EEPROM.write(i, 0);
    }

    finger.begin(57600);
    lcd.clear();
    lcd.print("Finding Module..");
    lcd.setCursor(0, 1);
    delay(2000);
    if (finger.verifyPassword())
    {
        Serial.println("Found fingerprint sensor!");
        lcd.clear();
        lcd.print(" Module Found");
        delay(2000);
    }
    else
    {
        Serial.println("Did not find fingerprint sensor :(");
        lcd.clear();
        lcd.print("Module Not Found");
        lcd.setCursor(0, 1);
        lcd.print("Check Connections");
        while (1)
            ;
    }

    if (!rtc.begin())
        Serial.println("Couldn't find RTC");

    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

    if (rtc.lostPower())
    {
        Serial.println("RTC is NOT running!");
        // following line sets the RTC to the date & time this sketch was compiled
        rtc.adjust(DateTime(2018, 6, 7, 11, 0, 0));
        // This line sets the RTC with an explicit date & time, for example to set
        // June 7, 2018 at 11am you would call:
        // rtc.adjust(DateTime(2018, 6, 7, 11, 0, 0));
    }

    //SD CARD

    if (!SD.begin(chipSelect))
    {
        Serial.println("Card failed, or not present");
        // don't do anything more:
        return;
    }else{
       Serial.println("card initialized.");
    }


    lcd.setCursor(0, 0);
    lcd.print(" Press Match to ");
    lcd.setCursor(0, 1);
    lcd.print(" Start System");
    delay(3000);

    user1 = EEPROM.read(1000);
    user2 = EEPROM.read(1001);
    user3 = EEPROM.read(1002);
    user4 = EEPROM.read(1003);
    user5 = EEPROM.read(1004);
    lcd.clear();
    digitalWrite(indFinger, HIGH);
}

void loop()
{
    now = rtc.now();
    lcd.setCursor(0, 0);
    lcd.print("Time: ");
    lcd.print(now.hour(), DEC);
    lcd.print(':');
    lcd.print(now.minute(), DEC);
    lcd.print(':');
    lcd.print(now.second(), DEC);
    lcd.print(" ");
    lcd.setCursor(0, 1);
    lcd.print("Date: ");
    lcd.print(now.day(), DEC);
    lcd.print('/');
    lcd.print(now.month(), DEC);
    lcd.print('/');
    lcd.print(now.year(), DEC);
    lcd.print(" ");
    delay(500);
    int result = getFingerprintIDez();
    if (result > 0)
    {
        digitalWrite(indFinger, LOW);
        digitalWrite(buzzer, HIGH);
        delay(100);
        digitalWrite(buzzer, LOW);
        lcd.clear();
        lcd.print("ID:");
        lcd.print(result);
        lcd.setCursor(0, 1);
        lcd.print("Please Wait....");
        delay(1000);
        attendance(result);
        lcd.clear();
        lcd.print("Attendance ");
        lcd.setCursor(0, 1);
        lcd.print("Registered");
        delay(1000);
        digitalWrite(indFinger, HIGH);
        return;
    }
    checkKeys();
    delay(300);
}

// dmyyhms - 7 bytes
void attendance(int id)
{
    int user = 0, eepLoc = 0;
    if (id == 1)
    {
        eepLoc = 0;
        user = user1++;
    }
    else if (id == 2)
    {
        eepLoc = 210;
        user = user2++;
    }
    else if (id == 3)
    {
        eepLoc = 420;
        user = user3++;
    }
    else if (id == 4)
    {
        eepLoc = 630;
        user = user4++;
    }
    else if (id == 5)
    {
        eepLoc = 0;
        user = user5++;
    }
    else if (id == 6)
    {
        eepLoc = 840;
        user = user5++;
    }
    else if (id == 7)
    {
        eepLoc = 1050;
        user = user7++;
    }
    else if (id == 8)
    {
        eepLoc = 1260;
        user = user8++;
    }
    else if (id == 9)
    {
        eepLoc = 1470;
        user = user9++;
    }
    else if (id == 10)
    {
        eepLoc = 1680;
        user = user8++;
    }
    /*else if(id == 5) // fifth user
{
eepLoc=840;
user=user5++;
}*/
    else
        return;

    int eepIndex = (user * 7) + eepLoc;
    EEPROM.write(eepIndex++, now.hour());
    EEPROM.write(eepIndex++, now.minute());
    EEPROM.write(eepIndex++, now.second());
    EEPROM.write(eepIndex++, now.day());
    EEPROM.write(eepIndex++, now.month());
    EEPROM.write(eepIndex++, now.year() >> 8);
    EEPROM.write(eepIndex++, now.year());

    EEPROM.write(1000, user1);
    EEPROM.write(1001, user2);
    EEPROM.write(1002, user3);
    EEPROM.write(1003, user4);
    // EEPROM.write(4,user5); // figth user
}

void checkKeys()
{
    if (lcd.button() == KEYPAD_SELECT)
    {
        lcd.clear();
        lcd.print("Please Wait");
        delay(1000);
        while (lcd.button() == KEYPAD_SELECT)
            ;
        Enroll();
    }

    else if (lcd.button() == KEYPAD_LEFT)
    {
        lcd.clear();
        lcd.print("Please Wait");
        delay(1000);
        delet();
    }
}

void Enroll()
{
    int count = 1;
    lcd.clear();
    lcd.print("Enter Finger ID:");

    while (1)
    {
        lcd.setCursor(0, 1);
        lcd.print(count);
        if (lcd.button() == KEYPAD_UP)
        {
            count++;
            if (count > records)
                count = 1;
            delay(500);
        }

        else if (lcd.button() == KEYPAD_DOWN)
        {
            count--;
            if (count < 1)
                count = records;
            delay(500);
        }
        else if (lcd.button() == KEYPAD_LEFT)
        {
            id = count;
            getFingerprintEnroll();
            for (int i = 0; i < records; i++)
            {
                if (EEPROM.read(i) != 0xff)
                {
                    EEPROM.write(i, id);
                    break;
                }
            }
            return;
        }

        else if (lcd.button() == KEYPAD_SELECT)
        {
            return;
        }
    }
}

void delet()
{
    int count = 1;
    lcd.clear();
    lcd.print("Enter Finger ID");

    while (1)
    {
        lcd.setCursor(0, 1);
        lcd.print(count);
        if (lcd.button() == KEYPAD_UP)
        {
            count++;
            if (count > records)
                count = 1;
            delay(500);
        }

        else if (lcd.button() == KEYPAD_DOWN)
        {
            count--;
            if (count < 1)
                count = records;
            delay(500);
        }
        else if (lcd.button() == KEYPAD_LEFT)
        {
            id = count;
            deleteFingerprint(id);
            for (int i = 0; i < records; i++)
            {
                if (EEPROM.read(i) == id)
                {
                    EEPROM.write(i, 0xff);
                    break;
                }
            }
            return;
        }

        else if (lcd.button() == KEYPAD_SELECT)
        {
            return;
        }
    }
}

uint8_t getFingerprintEnroll()
{
    int p = -1;
    lcd.clear();
    lcd.print("finger ID:");
    lcd.print(id);
    lcd.setCursor(0, 1);
    lcd.print("Place Finger");
    delay(2000);
    while (p != FINGERPRINT_OK)
    {
        p = finger.getImage();
        switch (p)
        {
        case FINGERPRINT_OK:
            Serial.println("Image taken");
            lcd.clear();
            lcd.print("Image taken");
            break;
        case FINGERPRINT_NOFINGER:
            Serial.println("No Finger");
            lcd.clear();
            lcd.print("No Finger Found");
            break;
        case FINGERPRINT_PACKETRECIEVEERR:
            Serial.println("Communication error");
            lcd.clear();
            lcd.print("Comm Error");
            break;
        case FINGERPRINT_IMAGEFAIL:
            Serial.println("Imaging error");
            lcd.clear();
            lcd.print("Imaging Error");
            break;
        default:
            Serial.println("Unknown error");
            lcd.clear();
            lcd.print("Unknown Error");
            break;
        }
    }

    // OK success!

    p = finger.image2Tz(1);
    switch (p)
    {
    case FINGERPRINT_OK:
        Serial.println("Image converted");
        lcd.clear();
        lcd.print("Image converted");
        break;
    case FINGERPRINT_IMAGEMESS:
        Serial.println("Image too messy");
        lcd.clear();
        lcd.print("Image too messy");
        return p;
    case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        lcd.clear();
        lcd.print("Comm Error");
        return p;
    case FINGERPRINT_FEATUREFAIL:
        Serial.println("Could not find fingerprint features");
        lcd.clear();
        lcd.print("Feature Not Found");
        return p;
    case FINGERPRINT_INVALIDIMAGE:
        Serial.println("Could not find fingerprint features");
        lcd.clear();
        lcd.print("Feature Not Found");
        return p;
    default:
        Serial.println("Unknown error");
        lcd.clear();
        lcd.print("Unknown Error");
        return p;
    }

    Serial.println("Remove finger");
    lcd.clear();
    lcd.print("Remove Finger");
    delay(2000);
    p = 0;
    while (p != FINGERPRINT_NOFINGER)
    {
        p = finger.getImage();
    }
    Serial.print("ID ");
    Serial.println(id);
    p = -1;
    Serial.println("Place same finger again");
    lcd.clear();
    lcd.print("Place Finger");
    lcd.setCursor(0, 1);
    lcd.print(" Again");
    while (p != FINGERPRINT_OK)
    {
        p = finger.getImage();
        switch (p)
        {
        case FINGERPRINT_OK:
            Serial.println("Image taken");
            break;
        case FINGERPRINT_NOFINGER:
            Serial.print(".");
            break;
        case FINGERPRINT_PACKETRECIEVEERR:
            Serial.println("Communication error");
            break;
        case FINGERPRINT_IMAGEFAIL:
            Serial.println("Imaging error");
            break;
        default:
            Serial.println("Unknown error");
            return 0;
        }
    }

    // OK success!

    p = finger.image2Tz(2);
    switch (p)
    {
    case FINGERPRINT_OK:
        Serial.println("Image converted");
        break;
    case FINGERPRINT_IMAGEMESS:
        Serial.println("Image too messy");
        return p;
    case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        return p;
    case FINGERPRINT_FEATUREFAIL:
        Serial.println("Could not find fingerprint features");
        return p;
    case FINGERPRINT_INVALIDIMAGE:
        Serial.println("Could not find fingerprint features");
        return p;
    default:
        Serial.println("Unknown error");
        return p;
    }

    // OK converted!
    Serial.print("Creating model for #");
    Serial.println(id);

    p = finger.createModel();
    if (p == FINGERPRINT_OK)
    {
        Serial.println("Prints matched!");
    }
    else if (p == FINGERPRINT_PACKETRECIEVEERR)
    {
        Serial.println("Communication error");
        return p;
    }
    else if (p == FINGERPRINT_ENROLLMISMATCH)
    {
        Serial.println("Fingerprints did not match");
        return p;
    }
    else
    {
        Serial.println("Unknown error");
        return p;
    }

    Serial.print("ID ");
    Serial.println(id);
    p = finger.storeModel(id);
    if (p == FINGERPRINT_OK)
    {
        Serial.println("Stored!");
        lcd.clear();
        lcd.print(" Finger Stored!");
        delay(2000);
    }
    else if (p == FINGERPRINT_PACKETRECIEVEERR)
    {
        Serial.println("Communication error");
        return p;
    }
    else if (p == FINGERPRINT_BADLOCATION)
    {
        Serial.println("Could not store in that location");
        return p;
    }
    else if (p == FINGERPRINT_FLASHERR)
    {
        Serial.println("Error writing to flash");
        return p;
    }
    else
    {
        Serial.println("Unknown error");
        return p;
    }
}

int getFingerprintIDez()
{
    uint8_t p = finger.getImage();

    if (p != FINGERPRINT_OK)
        return -1;

    p = finger.image2Tz();
    if (p != FINGERPRINT_OK)
        return -1;

    p = finger.fingerFastSearch();
    if (p != FINGERPRINT_OK)
    {
        lcd.clear();
        lcd.print("Finger Not Found");
        lcd.setCursor(0, 1);
        lcd.print("Try Later");
        delay(2000);
        return -1;
    }
    // found a match!
    Serial.print("Found ID #");
    Serial.print(finger.fingerID);
    SDCardRecord(finger.fingerID);
    return finger.fingerID;
}

void SDCardRecord(int id)
{
    String dataString = "";
    dataString += String(id);
    dataString += '\t';

    dataString += String(now.hour(),DEC);
    dataString += ':';
    dataString += String(now.minute(),DEC);
    dataString += ':';
    dataString += String(now.second(),DEC);
    dataString += '\t';
    dataString += String(now.day(),DEC);
    dataString += '/';
    dataString += String(now.month(),DEC);
    dataString += '/';
    dataString += String(now.year(),DEC);

    String fileName = String(now.day(),DEC);
    fileName += "_";
    fileName += String(now.month(),DEC);
    fileName += "_";
    fileName += String(now.year(),DEC);
    fileName += String(".txt");
    File dataFile = SD.open(fileName, FILE_WRITE);
    // File dataFile = SD.open("records.txt", FILE_WRITE);

    if (dataFile)
    {
        dataFile.println(dataString);
        dataFile.close();
        // print to the serial port too:
        Serial.println("");
        Serial.println(dataString);
        lcd.clear();
        lcd.print("Saved to");
        lcd.setCursor(0, 1);
        lcd.print("SDCard");
        delay(1000);
    }
    // if the file isn't open, pop up an error:
    else
    {
        lcd.clear();
        lcd.print("Error Opening");
        lcd.setCursor(0, 1);
        lcd.print(fileName);
        Serial.print("error opening ");
        Serial.println(fileName);
        delay(1000);
    }
}

uint8_t deleteFingerprint(uint8_t id)
{
    uint8_t p = -1;
    lcd.clear();
    lcd.print("Please wait");
    p = finger.deleteModel(id);
    if (p == FINGERPRINT_OK)
    {
        Serial.println("Deleted!");
        lcd.clear();
        lcd.print("Finger Deleted");
        lcd.setCursor(0, 1);
        lcd.print("Successfully");
        delay(1000);
    }

    else
    {
        Serial.print("Something Wrong");
        lcd.clear();
        lcd.print("Something Wrong");
        lcd.setCursor(0, 1);
        lcd.print("Try Again Later");
        delay(2000);
        return p;
    }
}

void download(int eepIndex)
{

    if (EEPROM.read(eepIndex) != 0xff)
    {
        Serial.print("T->");
        if (EEPROM.read(eepIndex) < 10)
            Serial.print('0');
        Serial.print(EEPROM.read(eepIndex++));
        Serial.print(':');
        if (EEPROM.read(eepIndex) < 10)
            Serial.print('0');
        Serial.print(EEPROM.read(eepIndex++));
        Serial.print(':');
        if (EEPROM.read(eepIndex) < 10)
            Serial.print('0');
        Serial.print(EEPROM.read(eepIndex++));
        Serial.print(" D->");
        if (EEPROM.read(eepIndex) < 10)
            Serial.print('0');
        Serial.print(EEPROM.read(eepIndex++));
        Serial.print('/');
        if (EEPROM.read(eepIndex) < 10)
            Serial.print('0');
        Serial.print(EEPROM.read(eepIndex++));
        Serial.print('/');
        Serial.print(EEPROM.read(eepIndex++) << 8 | EEPROM.read(eepIndex++));
    }
    else
    {
        Serial.print("---------------------------");
    }

    Serial.print(" ");
}
