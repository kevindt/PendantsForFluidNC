#include "EncoderN.h"

void init_encoderN(gpio_num_t _pulse_pin, gpio_num_t _control_pin, uint16_t _filter, pcnt_unit_t _unit, pcnt_channel_t _channel)
 {
    pcnt_config_t enc_config = {
        .pulse_gpio_num = GPIO_NUM_41,  //Rotary Encoder Chan A
        .ctrl_gpio_num  = GPIO_NUM_40,  //Rotary Encoder Chan B

        .lctrl_mode = PCNT_MODE_KEEP,     // Rising A on HIGH B = CW Step
        .hctrl_mode = PCNT_MODE_REVERSE,  // Rising A on LOW B = CCW Step
        .pos_mode   = PCNT_COUNT_DEC,     // Count Only On Falling-Edges
        .neg_mode   = PCNT_COUNT_INC,     // Discard Rising-Edges

        .counter_h_lim = INT16_MAX,
        .counter_l_lim = INT16_MIN,

        .unit    = PCNT_UNIT_0,
        .channel = PCNT_CHANNEL_0,
    };

    pcnt_unit_config(&enc_config);

    enc_config.pulse_gpio_num = _pulse_pin;
    enc_config.ctrl_gpio_num  = _control_pin;
    enc_config.unit           = _unit;
    enc_config.channel        = _channel;
    enc_config.pos_mode       = PCNT_COUNT_DEC;  
    enc_config.neg_mode       = PCNT_COUNT_INC;  
    
    pcnt_unit_config(&enc_config);

    // The length of ignored pulses is provided in APB_CLK clock cycles by calling pcnt_set_filter_value(). 
    // The current filter setting may be checked with pcnt_get_filter_value(). The APB_CLK clock is running at 80 MHz.
    pcnt_set_filter_value(_unit, _filter);  // Filter Runt Pulses (250 is reasonable value)

    // The filter is put into operation / suspended by calling pcnt_filter_enable() / pcnt_filter_disable().
    pcnt_filter_enable(_unit);

    gpio_pullup_en(_pulse_pin);
    gpio_pullup_en(_control_pin);

    pcnt_counter_pause(_unit);  // Initial PCNT init
    pcnt_counter_clear(_unit);
    pcnt_counter_resume(_unit);
 }

int16_t get_encoderN(pcnt_unit_t _unit) {
    int16_t count;
    pcnt_get_counter_value(_unit, &count);
    return count>>2; // four pulses per detent
 }
 