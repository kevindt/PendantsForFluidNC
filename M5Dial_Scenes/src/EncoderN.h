#pragma once

#include <Arduino.h>
#include "sdkconfig.h"
#include "driver/pcnt.h"
#include "driver/gpio.h"

void    init_encoderN(gpio_num_t _pulse_pin, gpio_num_t _control_pin, uint16_t _filter, pcnt_unit_t _unit, pcnt_channel_t _channel);
int16_t get_encoderN(pcnt_unit_t _unit);