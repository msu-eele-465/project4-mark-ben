#include <msp430fr2311.h>
#include "ledbar.h"

void ledbar_i2c_slave_setup() {
    P1SEL1 &= ~(BIT2 | BIT3);
    P1SEL0 |= (BIT2 | BIT3);

    UCB0CTLW0 |= UCSWRST;
    UCB0I2COA0 = 0x01 | UCOAEN;

    UCB0CTLW0 &= ~UCMST; 

    UCB0CTLW0 &= ~UCSWRST;

    UCB0IE |= UCRXIE;

    __enable_interrupt();
}

#pragma vector = USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void) {
    
    if(UCB0IV == 10) {
        /* unsigned char pattNum = UCB0RXBUF;      // Read received byte
        switch (pattNum) {
            case '0':
                change_led_pattern(0);
                break;
            case '1':
                change_led_pattern(1);
                break;
            case '2':
                change_led_pattern(2);
                break;
            case '3':
                change_led_pattern(3);
                break;
            case '4':
                if(base_tp > 0.25) {
                    base_tp -= 0.25;
                }
                break;
            case '5':
                base_tp += 0.25;
                break;
        } */

        uint8_t led_data = UCB0RXBUF;
        update_ledbar_pins(led_data);
    }
}