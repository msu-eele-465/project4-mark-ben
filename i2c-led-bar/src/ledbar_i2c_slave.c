#include <msp430fr2311.h>
#include "ledbar.h"

void ledbar_i2c_slave_setup() {
    P1SEL1 &= ~(BIT2 | BIT3);
    P1SEL0 |= (BIT2 | BIT3);



    UCB0CTLW0 &= ~UCSWRST;

    UCB0IE |= UCRXIE;

}

#pragma vector = USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void) {
    
}