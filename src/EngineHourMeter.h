#ifndef ENGINE_HOUR_METER_h
#define ENGINE_HOUR_METER_h

#include <Arduino.h>
#include <EEPROM.h>

#include "TimedAction.h"
#include "SegmentDisplay_Cathode47.hpp"

class EngineHourMeter {

public:
    EngineHourMeter();
    ~EngineHourMeter();
    
private:
    SegmentDisplay_Cathode47 *display;
};

#endif ENGINE_HOUE_METER_h