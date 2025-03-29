
MAIN
1.  Init:
        MSP430FR2355
        I2C MSP430FR2310
        keypad
        leds
        lcd

2.  Locked:
        LCD off
        Status LED locked state
        Poll keypad

3.  Unlocking:  
        Status LED unlocking state
        Wait for 4 digits
        If correct: 
            Status LED unlocked state
        If incorrect:
            Status LED locked state

4.  Unlocked:
        Enable LCD keypress display

            5.  Unlocked Keypad:
                Button Pressed:
                    I2C message to update LCD display
                        If setting/pattern button:
                            I2C message to update LED bar pattern
                            I2C message to update LCD pattern name

                            I2C message to update base transition period
                        If D:
                            locked
                            Status LED locked state

