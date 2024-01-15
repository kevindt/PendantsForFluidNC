#include "EncoderN.h"

void init_encoderN(gpio_num_t _PULSE_PIN, gpio_num_t _CONTROL_PIN, pcnt_count_mode_t _POS_MODE, pcnt_count_mode_t _NEG_MODE, uint16_t _FILTER, pcnt_unit_t _UNIT, pcnt_channel_t _CHANNEL)
 {
    pcnt_config_t enc_config = {
        //defaults for M5Dial

        // The PCNT module has eight independent counting “units” numbered from 0 to 7. In the API they are referred to using pcnt_unit_t. 
        // Each unit has two independent channels numbered as 0 and 1 and specified with pcnt_channel_t.
        .unit    = PCNT_UNIT_0,
        .channel = PCNT_CHANNEL_0,

        // GPIO numbers of the pulse input and the pulse gate input.
        // To disable the pulse or the control input pin in configuration, provide PCNT_PIN_NOT_USED instead of the GPIO number.
        .pulse_gpio_num = GPIO_NUM_40,  //Rotary Encoder Chan A
        .ctrl_gpio_num  = GPIO_NUM_41,  //Rotary Encoder Chan B

        // Two pairs of parameters: pcnt_ctrl_mode_t and cnt_count_mode_t define how the counter reacts 
        // depending on the the status of control signal and how counting is done relative to positive / negative edge of the pulses.
        .lctrl_mode = PCNT_MODE_KEEP,     // Rising A on HIGH B = CW Step
        .hctrl_mode = PCNT_MODE_REVERSE,  // Rising A on LOW B = CCW Step
        .pos_mode   = PCNT_COUNT_INC,     // Count Only On Rising-Edges
        .neg_mode   = PCNT_COUNT_DEC,     // Discard Falling-Edge
        
        // Two limit values (minimum / maximum) that are used to establish watchpoints and trigger interrupts when the pulse count is meeting particular limit.
        .counter_h_lim = INT16_MAX,
        .counter_l_lim = INT16_MIN,
 
    };
    pcnt_unit_config(&enc_config);

    enc_config.pulse_gpio_num = _PULSE_PIN;
    enc_config.ctrl_gpio_num  = _CONTROL_PIN;
    enc_config.unit           = _UNIT;
    enc_config.channel        = _CHANNEL;
    enc_config.pos_mode       = _POS_MODE;  
    enc_config.neg_mode       = _NEG_MODE;  
    
    pcnt_unit_config(&enc_config);

    // The length of ignored pulses is provided in APB_CLK clock cycles by calling pcnt_set_filter_value(). 
    // The current filter setting may be checked with pcnt_get_filter_value(). The APB_CLK clock is running at 80 MHz.
    pcnt_set_filter_value(_UNIT, _FILTER);  // Filter Runt Pulses (250 is reasonable value)

    // The filter is put into operation / suspended by calling pcnt_filter_enable() / pcnt_filter_disable().
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
 