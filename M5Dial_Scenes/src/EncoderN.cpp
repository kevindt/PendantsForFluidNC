#include "EncoderN.h"

void init_encoderN(gpio_num_t _PULSE_PIN, gpio_num_t _CONTROL_PIN, pcnt_count_mode_t _POS_MODE, pcnt_count_mode_t _NEG_MODE, uint16_t _FILTER, pcnt_unit_t _UNIT, pcnt_channel_t _CHANNEL)
 {
    pcnt_config_t enc_config = {
        //defaults for M5Dial
        .pulse_gpio_num = GPIO_NUM_40,  //Rotary Encoder Chan A
        .ctrl_gpio_num  = GPIO_NUM_41,  //Rotary Encoder Chan B

        .lctrl_mode = PCNT_MODE_KEEP,     // Rising A on HIGH B = CW Step
        .hctrl_mode = PCNT_MODE_REVERSE,  // Rising A on LOW B = CCW Step
        .pos_mode   = PCNT_COUNT_INC,     // Count Only On Rising-Edges
        .neg_mode   = PCNT_COUNT_DEC,     // Discard Falling-Edge

        .counter_h_lim = INT16_MAX,
        .counter_l_lim = INT16_MIN,

        .unit    = PCNT_UNIT_0,
        .channel = PCNT_CHANNEL_0,
    };
    pcnt_unit_config(&enc_config);

    enc_config.pulse_gpio_num = _PULSE_PIN;
    enc_config.ctrl_gpio_num  = _CONTROL_PIN;
    enc_config.unit           = _UNIT;
    enc_config.channel        = _CHANNEL;
    enc_config.pos_mode       = _POS_MODE;  
    enc_config.neg_mode       = _NEG_MODE;  
    
    pcnt_unit_config(&enc_config);

    pcnt_set_filter_value(_UNIT, _FILTER);  // Filter Runt Pulses

    pcnt_filter_enable(_UNIT);

    gpio_pullup_en(_PULSE_PIN);
    gpio_pullup_en(_CONTROL_PIN);

    pcnt_counter_pause(_UNIT);  // Initial PCNT init
    pcnt_counter_clear(_UNIT);
    pcnt_counter_resume(_UNIT);
 }

int16_t get_encoderN(pcnt_unit_t _UNIT) {
    int16_t count;
    pcnt_get_counter_value(_UNIT, &count);
    return count>>2; // four pulses per detent
 }
 