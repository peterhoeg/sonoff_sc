#ifndef _SAMPLE_DATA_H_
#define _SAMPLE_DATA_H_

#include "Arduino.h" // for delayMicroseconds, digitalPinToBitMask, etc
#include <inttypes.h>

void initDevice(void);
void getTempHumi(void);
void getAdcSensorValue(void);

#endif
