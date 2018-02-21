#include <Arduino.h>
#include <EEPROM.h>

#include "SegmentDisplay_Cathode47.hpp"
#include "TimedAction.h"

void buttonISR();
void incrementSecondsPassed();
void backUpDataToEEPROM();

void resetSecondsPassedPerm();
void resetSecondsPassedTemp();

uint32_t readIntegerFromEEPROM(const uint16_t& adress);
void putIntegerIntoEEPROM(uint32_t num, const uint16_t& adress);

const uint8_t serialDataPin = 8;
const uint8_t shiftClockPin = 12;
const uint8_t storageClockPin = 10;

const uint8_t digit1 = 3;
const uint8_t digit2 = 4;
const uint8_t digit3 = 5;
const uint8_t digit4 = 6;

const uint8_t buttonPin = 2;

const uint8_t ledPin = 7;

OutputPins displayPins = {
    digit1,
    digit2,
    digit3,
    digit4,
    serialDataPin,
    shiftClockPin,
    storageClockPin
};

const uint32_t LAST_LEFT_MINUTE = 359940;
const uint32_t MAX_SECONDS = 360000;
const uint16_t ADRESS_PERM = 0x0;
const uint16_t ADRESS_TEMP = 0x4;
const uint32_t BACK_UP_INTERVAL = 300000; // in milliseconds

uint32_t secondsPassedPerm;
uint32_t secondsPassedTemp;
bool resetTemp = false;
bool resetPerm = false;

volatile bool routine = true;

volatile uint64_t timeLastPush;
volatile uint64_t  timeLastInterrupt;
volatile uint16_t interuptsCount;

SegmentDisplay_Cathode47 display(&displayPins);

TimedAction threadMeter(1000, incrementSecondsPassed);
TimedAction threadSaver(BACK_UP_INTERVAL, backUpDataToEEPROM);

void buttonISR() {
    volatile uint64_t mills = millis();

    if (mills - timeLastInterrupt >= 200) {
        if (mills - timeLastPush >= 250 && mills - timeLastPush <= 500) {
            routine = !routine;
            interuptsCount = 0;
        } else {
            timeLastPush = mills;
        }

        interuptsCount++;
        timeLastInterrupt = mills;
        if (interuptsCount == 30) {
            if (routine == false) {
                resetSecondsPassedTemp();
            }
            interuptsCount = 0;
        }
    }
}

void backUpDataToEEPROM() {
    putIntegerIntoEEPROM(secondsPassedPerm, ADRESS_PERM);
    putIntegerIntoEEPROM(secondsPassedTemp, ADRESS_TEMP);
}

void incrementSecondsPassed() {
    resetPerm == false ? secondsPassedPerm++ : resetPerm = false; // Make sure, that the button interrupt caused while 
    resetTemp == false ? secondsPassedTemp++ : resetTemp = false; // the threadMeter is being delayed don't add extra seconds.

    if (secondsPassedPerm == 360000) {
        resetSecondsPassedPerm();
    }
}

void resetSecondsPassedPerm() {
    secondsPassedPerm = 0;
    resetPerm = true;
    putIntegerIntoEEPROM(0, ADRESS_PERM);
}

void resetSecondsPassedTemp() {
    secondsPassedTemp = 0;
    putIntegerIntoEEPROM(0, ADRESS_TEMP);
}

void putIntegerIntoEEPROM(uint32_t num, const uint16_t &adress) {
    EEPROM.put(adress, num);
}

uint32_t readIntegerFromEEPROM(const uint16_t &adress) {
    uint32_t temp = 0;
    EEPROM.get(adress, temp);

    return temp;
}

void setup() {
    pinMode(serialDataPin, OUTPUT);
    pinMode(shiftClockPin, OUTPUT);
    pinMode(storageClockPin, OUTPUT);

    pinMode(digit1, OUTPUT);
    pinMode(digit2, OUTPUT);
    pinMode(digit3, OUTPUT);
    pinMode(digit4, OUTPUT);

    pinMode(ledPin, OUTPUT);

    pinMode(buttonPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(buttonPin), buttonISR, HIGH);

   // Serial.begin(115200);

   // putIntegerIntoEEPROM(0, ADRESS_PERM);
   // putIntegerIntoEEPROM(255, ADRESS_TEMP);

    secondsPassedPerm = readIntegerFromEEPROM(ADRESS_PERM); //Loosing 1 second in 5 minutes
    secondsPassedTemp = readIntegerFromEEPROM(ADRESS_TEMP);

    secondsPassedPerm += (secondsPassedPerm / 300) + 1;
    secondsPassedTemp += (secondsPassedTemp / 300) + 1;

    //Serial.println(secondsPassedPerm);
    //Serial.println(secondsPassedTemp);
}

void loop() {
    threadMeter.check();
    threadSaver.check();

    if (routine) {
        digitalWrite(ledPin, LOW);
        if (secondsPassedPerm >= LAST_LEFT_MINUTE) {
            if (secondsPassedPerm % 2) {
                display.showNothing();
            } else {
                display.showTime(secondsPassedPerm, true);
            }
        } else {
            if (secondsPassedPerm == 0) {
                display.showNothing();
            } else {
                display.showTime(secondsPassedPerm, secondsPassedPerm % 2);
            }
        }
    } else {
        digitalWrite(ledPin, HIGH);
        if (secondsPassedTemp == 0) {
            display.showNothing();
        } else {
            display.showTime(secondsPassedTemp, secondsPassedTemp % 2);
        }
    }
}