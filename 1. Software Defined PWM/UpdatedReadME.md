Author: Joshua Guillermo
Updated: 2/25/2023

The purpose of this code is to create a system that controls the brightness of the LED on the MSP430FR2355 by PWM, or pulse width modulation. In order to do so the duty cycle, or the ratio of time the signal is a high value to the time the signal is a low value, can be changed.

The code utilizes 2 inputs in the 4.1 and 2.3 pins, outputs in the 1.0 (red LED) and 6.6 (green LED) pins. The 4.1 pin will control the red LED, while the 2.1 pin will control the green LED. Two timers in TB0 and TB1 are utilized in order to control both LEDs.

For both timers, ensure that Up mode is used. In both TB0 and TB1, one of CCR1 and CCR0 should be set equal to a value such as 10000, or 100% of the duty cycle. The other capture/compare register should be set to a variable named DutyCycle, which should be set to half of the 100% duty cycle value or 50%. In this case, that value is 5000.

Both Port 2 and 4 interrupts act the same way. If the capture/compare register set equal to the DutyCycle is greater than or equal to the 100% duty cycle value, it should be reset to a duty cycle of 0%. If it is not greater than or equal to it, the capture/compare register will increment and increase by 10%. In this case that 10% would be 1000. 

The timer interrupts act the same as well, with case statements utilized. In the case of the CCR1, the LED is set to a 0. In the case of a interrupt flag for the respective Timer B, the LED is set to a 1. 

