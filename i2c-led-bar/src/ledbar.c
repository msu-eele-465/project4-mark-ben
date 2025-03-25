#include <msp430fr2311.h>
#include <stdint.h>

volatile uint8_t pattern = -1; // Current pattern
volatile uint8_t step[4] = {0, 0, 0, 0}; // Current step in each pattern
volatile float base_tp = 0.5;    // Default 1.0s
//volatile uint16_t base_tp = 4096;

const int pattern_1[4] = {0b10101010, 0b10101010, 0b01010101, 0b01010101};  // Pattern 1
const int pattern_3[6] = {0b00011000, 0b00100100,   // Pattern 3
                          0b01000010, 0b10000001,
                          0b01000010, 0b00100100};


// Setup the timer for the ledbar
void setup_ledbar_timer() {
    TB0CTL |= (TBSSEL__ACLK | MC__UP | ID__4);     // ACLK, Up mode, div by 4
    TB0CCR0 =  (int) ((32768 * base_tp) / 4.0);  // Set interval 
    P1DIR |= (BIT0 | BIT1 | BIT4 | BIT5 | BIT6 | BIT7); // Setup all the pins
    P2DIR |= (BIT0 | BIT6);
    TB0CCTL0 |= CCIE;                    // Enable Timer B2 interrupt
    TB0CCTL0 &= ~CCIFG;
}


// Update the status of the led bar
void update_led_bar() {
    uint8_t led_pins = 0;
    
    // Check the current pattern
    switch(pattern) {
    case 0:
        led_pins = 0b10101010;  // Pattern 0
        break;
    case 1:
        led_pins = pattern_1[step[pattern]]; // Pattern 1
        step[pattern] = (step[pattern] + 1) % 4; // advance to the next step
        break;
    case 2:
        led_pins = step[pattern]; // Pattern 2 
        step[pattern] = (step[pattern] + 1) % 255; // advance to the next step
        break;
    case 3:
        led_pins = pattern_3[step[pattern]]; // Pattern 3
        step[pattern] = (step[pattern] + 1) % 6; // advance to the next step
        break; 
    }

    update_led_bar_pins(led_pins); // Update the LEDs
}

// Changes the current pattern
void change_led_pattern(int new_pattern) {
    if (new_pattern == pattern) {
        step[pattern] = 0;  // Just reset the step count if the same pattern is selected
    }

    pattern = new_pattern;
}


// Update all the ledbar pins based on the value passed in
void update_led_bar_pins(int pins) {
/*     int current_pins = pins;

    if ((current_pins & 0b00000001) == 1) {
        P1OUT |= BIT0;
    } else {
        P1OUT &= ~BIT0;
    }
    current_pins = current_pins >> 1;

    if ((current_pins & 0b00000001) == 1) {
        P1OUT |= BIT1;
    } else {
        P1OUT &= ~BIT1;
    }
    current_pins = current_pins >> 1;

    if ((current_pins & 0b00000001) == 1) {
        P1OUT |= BIT4;
    } else {
        P1OUT &= ~BIT4;
    }
    current_pins = current_pins >> 1;

    if ((current_pins & 0b00000001) == 1) {
        P1OUT |= BIT5;
    } else {
        P1OUT &= ~BIT5;
    }
    current_pins = current_pins >> 1;

    if ((current_pins & 0b00000001) == 1) {
        P1OUT |= BIT6;
    } else {
        P1OUT &= ~BIT6;
    }
    current_pins = current_pins >> 1;

    if ((current_pins & 0b00000001) == 1) {
        P1OUT |= BIT7;
    } else {
        P1OUT &= ~BIT7;
    }
    current_pins = current_pins >> 1;

    if ((current_pins & 0b00000001) == 1) {
        P2OUT |= BIT0;
    } else {
        P2OUT &= ~BIT0;
    }
    current_pins = current_pins >> 1;

    if ((current_pins & 0b00000001) == 1) {
        P2OUT |= BIT6;
    } else {
        P2OUT &= ~BIT6;
    } */

    // Optimized for memory

    const volatile unsigned char * const ports[8] = {&P1OUT, &P1OUT, &P1OUT, &P1OUT, &P1OUT, &P1OUT, &P2OUT, &P2OUT};

    const unsigned led_bits[8] = {BIT0, BIT1, BIT4, BIT5, BIT6, BIT7, BIT0, BIT6};

    int i;
    for (i = 0; i < 8; i++) {
        if (pins & 0x01) {
            *(volatile unsigned char *)ports[i] |= led_bits[i];
        } else {
            *(volatile unsigned char *)ports[i] &= ~led_bits[i];
        }

        pins >>= 1;
    }
}

#pragma vector=TIMER0_B0_VECTOR
__interrupt void Timer_B0_ISR(void) {

    TB0CCTL0 &= ~CCIFG;

    update_led_bar(); // Update the status of the ledbar
    
    TB0CCTL0 &= ~CCIE;                              // Disable Timer B2 interrupt
    TB0CCR0 = (int) ((32768 * base_tp) / 4.0);      // Keep the rate of the clock up to date
    TB0CCTL0 |= CCIE;                               // Enable Timer B2 interrupt
    TB0CCTL0 &= ~CCIFG;
}