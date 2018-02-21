#ifndef SegmentDisplay_Cathode47_hpp
#define SegmentDisplay_Cathode47_hpp

#include <Arduino.h>

namespace {
    typedef const int DigitPin;

    enum class Digit {
        First,
        Second,
        Third,
        Fourth
    };

    typedef struct {
        DigitPin D1;
        DigitPin D2;
        DigitPin D3;
        DigitPin D4;

        DigitPin SER;
        DigitPin SRCLK;
        DigitPin RCLK;
    
    } OutputPins;

    const byte MASK_SIZE = 7;

    const byte bitMask[] = {
        B0111111,  //0
        B0000110,  //1
        B1011011,  //2
        B1001111,  //3
        B1100110,  //4
        B1101101,  //5
        B1111101,  //6
        B0000111,  //7
        B1111111,  //8
        B1101111,  //9
        B0000000, //nothing
    };

    class SegmentDisplay_Cathode47 {

    public:

        SegmentDisplay_Cathode47();
        SegmentDisplay_Cathode47(OutputPins const *pins);
        ~SegmentDisplay_Cathode47();

        void showInteger(int number);
        void showTime(uint32_t totalMinutes, bool dot);
        void showTime(int hours, int minutes, bool dot);
        void showNothing();

        void showDigit(Digit d, byte digit, bool dot); 

        void setOutputPins(OutputPins const *pins);
        void setTimeDelta(int time_delta);
        
    private:

        OutputPins const *outputPins;
        int time_delta;
        
        void turnOffAllSegments();
        byte getLastDigit(int &num);
    };

    SegmentDisplay_Cathode47::SegmentDisplay_Cathode47(OutputPins const *pins) : 
        outputPins(pins) {

    }

    SegmentDisplay_Cathode47::~SegmentDisplay_Cathode47() {}

    void SegmentDisplay_Cathode47::setTimeDelta(int time_delta) {
        this->time_delta = time_delta;
    }

    void SegmentDisplay_Cathode47::setOutputPins(OutputPins const *pins) {
        this->outputPins = pins;
    }

    void SegmentDisplay_Cathode47::showDigit(Digit d, byte digit, bool dot) {
        switch (d) {
            case Digit::First:
                digitalWrite(this->outputPins->D1, LOW);
                digitalWrite(this->outputPins->D2, HIGH);
                digitalWrite(this->outputPins->D3, HIGH);
                digitalWrite(this->outputPins->D4, HIGH);
                break;
            
            case Digit::Second:
                digitalWrite(this->outputPins->D1, HIGH);
                digitalWrite(this->outputPins->D2, LOW);
                digitalWrite(this->outputPins->D3, HIGH);
                digitalWrite(this->outputPins->D4, HIGH);
                break;

            case Digit::Third:
                digitalWrite(this->outputPins->D1, HIGH);
                digitalWrite(this->outputPins->D2, HIGH);
                digitalWrite(this->outputPins->D3, LOW);
                digitalWrite(this->outputPins->D4, HIGH);
                break;
                
            case Digit::Fourth:
                digitalWrite(this->outputPins->D1, HIGH);
                digitalWrite(this->outputPins->D2, HIGH);
                digitalWrite(this->outputPins->D3, HIGH);
                digitalWrite(this->outputPins->D4, LOW);
                break;
        }

        digitalWrite(this->outputPins->RCLK, LOW);

        for (int i = 0; i < MASK_SIZE; i++) {
            digitalWrite(this->outputPins->SRCLK, LOW);
            digitalWrite(this->outputPins->SER, bitMask[digit] & (1 << i));
            digitalWrite(this->outputPins->SRCLK, HIGH);
        }

        digitalWrite(this->outputPins->SRCLK, LOW);
        digitalWrite(this->outputPins->SER, dot);
        digitalWrite(this->outputPins->SRCLK, HIGH);

        digitalWrite(this->outputPins->RCLK, HIGH);

        this->turnOffAllSegments();
    }

    void SegmentDisplay_Cathode47::turnOffAllSegments() {
        digitalWrite(this->outputPins->RCLK, LOW);
        
        for (int i = 0; i < 8; i++) {
            digitalWrite(this->outputPins->SRCLK, LOW);
            digitalWrite(this->outputPins->SER, LOW);
            digitalWrite(this->outputPins->SRCLK, HIGH);
        }

        digitalWrite(this->outputPins->RCLK, HIGH);
    }

    byte SegmentDisplay_Cathode47::getLastDigit(int &num) {
        byte mod = num % 10;
        num /= 10;

        return mod;
    }

    void SegmentDisplay_Cathode47::showInteger(int num) {
        showDigit(Digit::Fourth, getLastDigit(num), false);
        showDigit(Digit::Third, getLastDigit(num), false);
        showDigit(Digit::Second, getLastDigit(num), false);
        showDigit(Digit::First, getLastDigit(num), false);
    }

    void SegmentDisplay_Cathode47::showTime(uint32_t totalSeconds, bool dot) {
        int hours = totalSeconds / 3600;
        int minutes = (totalSeconds / 60) % 60;

        showDigit(Digit::Fourth, getLastDigit(minutes), false);
        showDigit(Digit::Third, getLastDigit(minutes), false);
        showDigit(Digit::Second, getLastDigit(hours), dot); //Digit with dot underneath
        showDigit(Digit::First, getLastDigit(hours), false);
    }

    void SegmentDisplay_Cathode47::showTime(int hours, int minutes, bool dot) {
        showDigit(Digit::Fourth, getLastDigit(minutes), false);
        showDigit(Digit::Third, getLastDigit(minutes), false);
        showDigit(Digit::Second, getLastDigit(hours), dot); // Digit with dot underneath
        showDigit(Digit::First, getLastDigit(hours), false);
    }

    void SegmentDisplay_Cathode47::showNothing() {
        showDigit(Digit::Fourth, 10, false);
        showDigit(Digit::Third, 10, false);
        showDigit(Digit::Second, 10, false); // Digit with dot underneath
        showDigit(Digit::First, 10, false);
    }
}

#endif SegmentDisplay_Cathode47_hpp
