#ifndef LEDBAR_H
#define LEDBAR_H

extern volatile float base_tp;

void setup_ledbar_timer(void);
void ledbar_i2c_slave_setup(void);
void change_led_pattern(int new_pattern);

// Other function prototypes or definitions you want to share

#endif