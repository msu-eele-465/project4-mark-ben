#include "msp430fr2355.h"
#include <msp430.h>

void i2c_master_setup(void) {

    //-- Configure GPIO --------
    P1SEL1 &= ~(BIT2 | BIT3);           // eUSCI_B0
    P1SEL0 |= (BIT2 | BIT3);


    //-- eUSCI_B0 --
    UCB0CTLW0 = UCSWRST;

    UCB0CTLW0 |= UCSSEL_3;              // SMCLK
    UCB0BRW = 10;                     // Divider

    UCB0CTLW0 |= UCMODE_3;              // I2C Mode
    UCB0CTLW0 |= UCMST;                 // Master
    UCB0CTLW0 |= UCTR;                  // Tx
    
    
    UCB0CTLW0 &= ~UCSWRST;              // Take out of reset

}

void i2c_write_lcd(unsigned int pattNum, char character) {
    UCB0I2CSA = 0x02;
    UCB0TBCNT = 0x02;                   // Number of bytes
    UCB0CTLW0 |= UCTR;                  // Transmit Mode
    UCB0CTLW0 |= UCTXSTT;               // Generate Start Condition

    while (!(UCB0IFG & UCTXIFG));       // Wait for TX Buffer
    UCB0TXBUF = pattNum;                // Send data byte

    while (!(UCB0IFG & UCTXIFG));       // Wait for TX Buffer
    UCB0TXBUF = character;                // Send data byte

    while (!(UCB0IFG & UCTXIFG));       // Wait for TX Buffer
    UCB0CTLW0 |= UCTXSTP;               // Stop condition

}

void i2c_write_led(unsigned int pattNum) {
    UCB0I2CSA = 0x01;
    UCB0TBCNT = 0x01;                   // Number of bytes
    UCB0CTLW0 |= UCTR;                  // Transmit Mode
    UCB0CTLW0 |= UCTXSTT;               // Generate Start Condition

    while (!(UCB0IFG & UCTXIFG));       // Wait for TX Buffer
    UCB0TXBUF = pattNum;                   // Send data byte

    while (!(UCB0IFG & UCTXIFG));       // Wait for TX Buffer
    UCB0CTLW0 |= UCTXSTP;               // Stop condition

}