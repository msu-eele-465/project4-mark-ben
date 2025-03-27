#include "msp430fr2355.h"
#include <msp430.h>
#include "i2c_master.h"


void i2c_master_setup(void) {
    //-- eUSCI_B0 --
    UCB0CTLW0 |= UCSWRST;

    UCB0CTLW0 |= UCSSEL__SMCLK;              // SMCLK
    UCB0BRW = 10;                       // Divider

    UCB0CTLW0 |= UCMODE_3;              // I2C Mode
    UCB0CTLW0 |= UCMST;                 // Master
    UCB0CTLW0 |= UCTR;                  // Tx
    UCB0CTLW1 |= UCASTP_2;
    //-- Configure GPIO --------
    P1SEL1 &= ~BIT3;           // eUSCI_B0
    P1SEL0 |= BIT3;

    P1SEL1 &= ~BIT2;
    P1SEL0 |= BIT2;

    
}

void i2c_write_lcd(unsigned int pattNum, char character) {
    UCB0CTLW0 |= UCSWRST;
    UCB0CTLW0 |= UCTR;
    UCB0I2CSA = 0x0002;
    UCB0TBCNT = 0x02;                   // Number of bytes
    UCB0CTLW0 &= ~UCSWRST;
    UCB0IE |= UCTXIE0;

    send_buff = pattNum;                // Send data byte
    UCB0CTLW0 |= UCTXSTT;               // Generate Start Condition

    
    while (!(ready_to_send));       // Wait for TX Buffer
    ready_to_send = 0;
    


    send_buff = (int)character;                // Send data byte
    while (!(ready_to_send));       // Wait for TX Buffer
    ready_to_send = 0;

    //while((UCB0IFG & UCSTPIFG) == 0){};
    //    UCB0IFG &= ~UCSTPIFG;
}

void i2c_write_led(unsigned int pattNum) {
    UCB0CTLW0 |= UCSWRST;
    UCB0CTLW0 |= UCTR;
    UCB0I2CSA = 0x0040;
    UCB0TBCNT = 1;                      // Number of bytes
    
    
    UCB0CTLW0 &= ~UCSWRST;
    UCB0IE |= UCTXIE0;
    send_buff = pattNum;                // Send data byte
    UCB0CTLW0 |= UCTXSTT;               // Generate Start Condition
    
    while (!(ready_to_send));           // Wait for TX Buffer
    ready_to_send = 0;
    //while (!(UCB0IFG & UCSTPIFG));       // Wait for TX Buffer
    //UCB0CTLW0 &= ~UCTXSTP;               // Stop condition

}