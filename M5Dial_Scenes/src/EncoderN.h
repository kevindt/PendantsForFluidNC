#pragma once

#include <Arduino.h>
#include "sdkconfig.h"
#include "driver/pcnt.h"
#include "driver/gpio.h"

void    init_encoderN(gpio_num_t _PULSE_PIN, gpio_num_t _CONTROL_PIN, pcnt_count_mode_t _POS_MODE, pcnt_count_mode_t _NEG_MODE, uint16_t _FILTER, pcnt_unit_t _UNIT, pcnt_channel_t _CHANNEL);
int16_t get_encoderN(pcnt_unit_t _UNIT);