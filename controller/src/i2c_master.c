#include "msp430fr2355.h"
#include <msp430.h>

void i2c_master_setup(void) {

    //-- Configure GPIO --------
    P1SEL1 &= ~(BIT2 | BIT3);           // eUSCI_B0
    P1SEL0 |= (BIT2 | BIT3);

    P4SEL4 &= ~(BIT6 | BIT7);           // eUSCI_B1
    P4SEL0 |= (BIT6 | BIT7);


}