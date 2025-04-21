#pragma once

#include <stdint.h>

using time_t = uint16_t;

void clockInit();
time_t getTime();
void setTime(time_t time);
