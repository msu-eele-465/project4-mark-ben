/* --COPYRIGHT--,BSD_EX
 * Copyright (c) 2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************
 *
 *                       MSP430 CODE EXAMPLE DISCLAIMER
 *
 * MSP430 code examples are self-contained low-level programs that typically
 * demonstrate a single peripheral function or device feature in a highly
 * concise manner. For this the code may rely on the device's power-on default
 * register values and settings such as the clock configuration and care must
 * be taken when combining code from several examples to avoid potential side
 * effects. Also see www.ti.com/grace for a GUI- and www.ti.com/msp430ware
 * for an API functional library-approach to peripheral configuration.
 *
 * --/COPYRIGHT--*/
//******************************************************************************
//  MSP430FR235x Demo - Toggle P1.0 using software
//
//  Description: Toggle P1.0 every 0.1s using software.
//  By default, FR235x select XT1 as FLL reference.
//  If XT1 is present, the PxSEL(XIN & XOUT) needs to configure.
//  If XT1 is absent, switch to select REFO as FLL reference automatically.
//  XT1 is considered to be absent in this example.
//  ACLK = default REFO ~32768Hz, MCLK = SMCLK = default DCODIV ~1MHz.
//
//           MSP430FR2355
//         ---------------
//     /|\|               |
//      | |               |
//      --|RST            |
//        |           P1.0|-->LED
//
//   Cash Hao
//   Texas Instruments Inc.
//   November 2016
//   Built with IAR Embedded Workbench v6.50.0 & Code Composer Studio v6.2.0
//******************************************************************************
#include <msp430.h>
#include "../src/i2c_master.h"
#include <string.h>
#include <stdint.h>
#include "../src/statusled.h"

volatile int state_variable = 0;
char keypad_input[4] = {};
volatile int input_index = 0;
volatile int send_i2c_update = 0;


volatile int pattern = -1; // Current pattern
volatile int step[4] = {0, 0, 0, 0}; // Current step in each pattern
volatile float base_tp = 0.5;    // Default 1.0s

const uint8_t pattern_0 = 0b10101010;
const int pattern_1[4] = {0b10101010, 0b10101010, 0b01010101, 0b01010101};  // Pattern 1
const int pattern_3[6] = {0b00011000, 0b00100100,   // Pattern 3
                          0b01000010, 0b10000001,
                          0b01000010, 0b00100100};

void setup_heartbeat() {
    // --    LED   --
    
    P6DIR |= BIT6;                                          // P6.6 as OUTPUT
    P6OUT |= BIT6;                                          // Start LED off

    // -- Timer B0 --
    TB0R = 0;
    TB0CCTL0 = CCIE;                                        // Enable Interrupt
    TB0CCR0 = 32820;                                        // 1 sec timer
    TB0EX0 = TBIDEX__8;                                     // D8
    TB0CTL = TBSSEL__SMCLK | MC__UP | ID__4;                // Small clock, Up counter,  D4
    TB0CCTL0 &= ~CCIFG;
}

void rgb_timer_setup() {
    P3DIR |= (BIT2 | BIT7);                                 // Set as OUTPUTS
    P2DIR |= BIT4;
    P3OUT |= (BIT2 | BIT7);                                 // Start HIGH
    P2OUT |= BIT4;

    TB2R = 0;
    TB2CTL |= (TBSSEL__SMCLK | MC__UP);                     // Small clock, Up counter
    TB2CCR0 = 512;                                          // 1 sec timer
    TB2CCTL0 |= CCIE;                                       // Enable Interrupt
    TB2CCTL0 &= ~CCIFG;
}

void setup_ledbar_update_timer() {
    TB1CTL = TBSSEL__ACLK | MC__UP | ID__4;     // Use ACLK, up mode, divider 4
    TB1CCR0 = (int)((32768 * base_tp) / 4.0);                          // Set update interval based on base_tp
    TB1CCTL0 = CCIE;                            // Enable interrupt for TB1 CCR0
}

uint8_t compute_ledbar() {
    uint8_t led_pins = 0;
    switch (pattern) {
        case 0:
            led_pins = pattern_0;
            break;
        case 1:
            led_pins = pattern_1[step[pattern]];
            step[pattern] = (step[pattern] + 1) % 4;
            break;
        case 2:
            led_pins = step[pattern];
            step[pattern] = (step[pattern] + 1) % 255;
            break;
        case 3:
            led_pins = pattern_3[step[pattern]]; // Pattern 3
            step[pattern] = (step[pattern] + 1) % 6; // advance to the next step
            break;
        default:
            break; 
    }
    return led_pins;
}

void change_led_pattern(int new_pattern) {
    if (new_pattern == pattern) {
        step[pattern] = 0;  // Just reset the step count if the same pattern is selected
    }

    pattern = new_pattern;
}

void update_slave_ledbar() {
    volatile ledbar_pattern = compute_ledbar();
    //i2c_write_led(ledbar_pattern);
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer
    
    i2c_master_setup();
    setup_keypad();
    setup_heartbeat();
    setup_ledbar_update_timer();
    rgb_timer_setup();



    send_buff = 0;
    ready_to_send = 0;




    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode
                                            // to activate previously configured port settings
    UCB0CTLW0 &= ~UCSWRST;              // Take out of reset
    UCB0IE |= UCTXIE0;
    //UCB0IE |= UCRXIE0;

    __enable_interrupt();

    while(1)
    {


        char key = pressed_key();
        if (state_variable == 0 || state_variable == 2) {                      // Locked
            
            if (key != '\0') {                                                 // Check for key
                
                state_variable = 2;                                            // if key, unlocking
                if (input_index < 3) {                                         
                    keypad_input[input_index] = key;
                    input_index++;
                } else if (input_index == 3) {                                 // if 4 keys, check unlock
                    
                    check_key();
                }
            }   
        } else if(state_variable == 1) {
            if (key != '\0') {
                send_i2c_update = 1;
            }
           switch (key) {                                          // Lock if D, otherwise update pattern/base transition period
                case 'D':     
                    i2c_write_lcd(10, ' ');                                              
                    //change_led_pattern(-1);
                    //i2c_write_led(6);
                    state_variable = 0;
                    input_index = 0;
                    send_i2c_update = 0;                
                    memset(keypad_input, 0, sizeof(keypad_input));  // Clear input
                    break;
                case '0':
                    i2c_write_lcd(0, '0');
                    //change_led_pattern(0);
                    break;
                case '1':
                    i2c_write_lcd(1, '1');
                    //change_led_pattern(1);
                    break;
                case '2':
                    i2c_write_lcd(2, '2');
                    //change_led_pattern(2);
                    break;
                case '3':
                    i2c_write_lcd(3, '3');
                    //change_led_pattern(3);
                    break;
                case '4':
                    i2c_write_lcd(4, '4');
                    break;
                case '5':
                    i2c_write_lcd(5, '5');
                    break;
                case '6':
                    i2c_write_lcd(6, '6');
                    break;
                case '7':
                    i2c_write_lcd(7, '7');
                    break;
                case '8':
                    i2c_write_lcd(11, '8');
                    break;
                case '9':
                    i2c_write_lcd(11, '9');
                    break;                        
                case 'A':
                    i2c_write_lcd(11, 'A');
                    if (base_tp > 0.25) {
                        base_tp -= 0.25;
                    }
                    break;
                case 'B':
                    i2c_write_lcd(11, 'B');
                    base_tp += 0.25;
                    break;
                case 'C':
                    i2c_write_lcd(11, 'C');
                    break;
                case '*':
                    i2c_write_lcd(11, '*');
                    break;
                case '#':
                    i2c_write_lcd(11, '#');
                    break;                                                                                                 
                default:
                    input_index = 0;
                    memset(keypad_input, 0, sizeof(keypad_input));  // Clear input
                    break;
           }
           input_index = 0;
           memset(keypad_input, 0, sizeof(keypad_input));
        }
        update_led();
    }
}

#pragma vector=TIMER0_B0_VECTOR
__interrupt void Timer_B0_ISR(void) {
    TB0CCTL0 &= ~CCIFG;
    P6OUT ^= BIT6;
}

#pragma vector = TIMER1_B0_VECTOR
__interrupt void Timer_B1_ISR(void) {
    TB1CCTL0 &= ~CCIFG;

    if (state_variable == 1 && send_i2c_update) {
        //update_slave_ledbar();
        
    }
    
    TB1CCR0 = (int)((32768 * base_tp) / 4.0);
}

#pragma vector=EUSCI_B0_VECTOR
__interrupt void EUSCI_B0_ISR(void){
    int current = UCB0IV;
        UCB0TXBUF = send_buff;
        ready_to_send = 1;

}
