#ifndef I2C_MASTER_H
#define I2C_MASTER_H

#include <msp430.h>

void i2c_master_setup(void);

void i2c_write_led(unsigned int pattNum);
void i2c_write_lcd(unsigned int pattNum, char character);

#endif