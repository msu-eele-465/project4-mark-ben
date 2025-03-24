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
#include "../src/keypad.h"
#include "../src/i2c_master.h"

#define LEDBAR 0X01
#define LCD 0X02


volatile int state_variable = 0;
char keypad_input[4] = {};
volatile int input_index = 0;

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

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer
    
    setup_keypad();
    setup_heartbeat();

    P1OUT &= ~BIT0;                         // Clear P1.0 output latch for a defined power-on state
    P1DIR |= BIT0;                          // Set P1.0 to output direction

    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode
                                            // to activate previously configured port settings

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
           switch (key) {                                          // Lock if D, otherwise update pattern/base transition period
                case 'D':                                                   
                    i2c_write_led(6);
                    state_variable = 0;
                    input_index = 0;
                    //change_led_pattern(-1);                         
                    memset(keypad_input, 0, sizeof(keypad_input));  // Clear input
                    break;
                case '0':
                    i2c_write_led(0);
                    i2c_write_lcd(0, '0');
                    //change_led_pattern(0);
                    break;
                case '1':
                    i2c_write_led(1);
                    i2c_write_lcd(1, '1');
                    //change_led_pattern(1);
                    break;
                case '2':
                    i2c_write_led(2);
                    i2c_write_lcd(2, '2');
                    //change_led_pattern(2);
                    break;
                case '3':
                    i2c_write_led(3);
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
                    i2c_write_lcd(8, '8');
                    break;
                case '9':
                    i2c_write_lcd(9, '9');
                    break;                        
                case 'A':
                    i2c_write_led(4);
                    i2c_write_lcd(0, 'A');
                    //if (base_tp > 0.25) {
                    //    base_tp -= 0.25;
                    break;
                case 'B':
                    i2c_write_led(5);
                    i2c_write_lcd(0, 'B');
                    //base_tp += 0.25;
                    break;
                case 'C':
                    i2c_write_lcd(0, 'C');
                    //base_tp += 0.25;
                    break;
                case '*':
                    i2c_write_lcd(0, '*');
                    //base_tp += 0.25;
                    break;
                case '#':
                    i2c_write_lcd(0, '#');
                    //base_tp += 0.25;
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