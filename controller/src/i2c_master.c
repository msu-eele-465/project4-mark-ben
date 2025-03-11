#include "msp430fr2355.h"
#include <msp430.h>

void i2c_master_setup(void) {

    //-- Configure GPIO --------
    P1SEL1 &= ~(BIT2 | BIT3);           // eUSCI_B0
    P1SEL0 |= (BIT2 | BIT3);

    P4SEL4 &= ~(BIT6 | BIT7);           // eUSCI_B1
    P4SEL0 |= (BIT6 | BIT7);

    //-- eUSCI_B0 --
    UCB0CTLW0 = UCSWRST;

    UCB0CTLW0 |= UCSSEL_3;              // SMCLK
    UCB0CTLW0 = 10;                     // Divider

    UCB0CTLW0 |= UCMODE_3;              // I2C Mode
    UCB0CTLW0 |= UCMST;                 // Master
    UCB0CTLW0 |= UCTR;                  // Tx
    UCB0TBCNT = 0x02;                   // Number of bytes
    
    UCB0CTLW0 &= ~UCSWRST;              // Take out of reset

/*     //-- eUSCI_B1 --
    UCB1CTLW0 = UCSWRST;

    UCB1CTLW0 |= UCSSEL_3;              // SMCLK
    UCB1CTLW0 = 10;                     // Divider

    UCB1CTLW0 |= UCMODE_3;              // I2C Mode
    UCB1CTLW0 |= UCMST;                 // Master
    UCB1CTLW0 |= UCTR;                  // Tx
    

    UCB1TBCNT = sizeof(packet);         // packet size

    UCB1CTLW0 &= ~UCSWRST;              // Take out of reset */


}

void i2c_write(unsigned int slaveAddress, unsigned int data) {
    UCB0I2CSA = slaveAddress;
    UCB0CTLW0 |= UCTR;                  // Transmit Mode
    UCB0CTLW0 |= UCTXSTT;               // Generate Start Condition

    while (!(UCB0IFG & UCTXIFG));       // Wait for TX Buffer
    UCB0TXBUF = data;                   // Send data byte

    while (!(UCB0IFG & UCTXIFG));       // Wait for TX Buffer
    UCB0CTLW0 |= UCTXSTP;               // Stop condition
}