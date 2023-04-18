/*
 * part2.c
 *
 *  Created on: Feb 18, 2023
 *      Author: Joshua Guillermo
 */

#define PERIOD 1000
#define RED_GREEN 0
#define GREEN_BLUE 1
#define BLUE_RED 2

void LEDInit();
void TimerInit();

#include <msp430.h>

char LEDColor = RED_GREEN;

void main()
{
    WDTCTL = WDTPW | WDTHOLD;                    // Stop WDT

    LEDInit();
    TimerInit();

    PM5CTL0 &= ~LOCKLPM5;                         // Disable the GPIO power-on default high-impedance mode to activate previously configured port settings
    __bis_SR_register(LPM3_bits | GIE);           // Enter LPM3, enable interrupts
        __no_operation();                         // For debugger
}

     void LEDInit()
     {
         P6DIR |= BIT0;                           // P6.0 = Red LED
         P6DIR |= BIT1;                           // P6.1 = Blue LED
         P6DIR |= BIT2;                           // P6.2 = Green LED

         // Initializing used output pins to power-off state
         P6OUT &= ~BIT0;
         P6OUT &= ~BIT1;
         P6OUT &= ~BIT2;

         // Initializing the select pins
         P6SEL0 |= BIT0;
         P6SEL0 |= BIT1;
         P6SEL0 |= BIT2;
         P6SEL1 &= ~BIT0;
         P6SEL1 &= ~BIT1;
         P6SEL1 &= ~BIT2;

         // Enabling the interrupts for output pins
         P6IE |= BIT0;
         P6IE |= BIT1;
         P6IE |= BIT2;
     }

     void TimerInit()
     {
         TB3CCR0 = PERIOD-1;                              // PWM period
         TB3CTL = TBSSEL__SMCLK | MC__UP | TBCLR;
         TB3CCTL1 = OUTMOD_7;                           // Reset/set CCR1
         TB3CCTL2 = OUTMOD_7;                           // Reset/set CCR2
         TB3CCTL3 = OUTMOD_7;                           // Reset/set CCR3

         // Initial duty cycle for each LED
         TB3CCR1 = PERIOD-1;                              // Red LED on
         TB3CCR2 = 0;                                   // Blue LED off
         TB3CCR3 = 0;                                   // Green LED off

         // Setup timer for fade
         TB1CCTL0 |= CCIE;                                  // Enable interrupt when overflow
         TB1CCR0 = 1;                                       // Initial period of timer
         TB1CTL = TBSSEL_1 | MC_2 | ID_3 | TBCLR | TBIE;    // ACLK, continuous mode, divide by 8, clear timer, enable interrupt
     }

// TimerB1 interrupt
#pragma vector = TIMER1_B0_VECTOR
__interrupt void TIMER1_B0_ISR(void)
{
    switch(LEDColor)
    {
    case RED_GREEN:                 // Red fades to Green
        TB3CCR1--;                  // decrease Red value
        TB3CCR3++;                  // increase Green value
        if(TB3CCR1 == 0)
        {
            LEDColor = GREEN_BLUE;
        }
        break;

    case GREEN_BLUE:                // Green fades to Blue
        TB3CCR3--;                  // decrease Green value
        TB3CCR2++;                  // increase Blue value
        if(TB3CCR3 == 0)
        {
            LEDColor = BLUE_RED;
        }
        break;

    case BLUE_RED:                  // Blue fades to Red
        TB3CCR2--;                  // decrease Blue value
        TB3CCR1++;                  // increase Red value
        if(TB3CCR2 == 0)
        {
            LEDColor = RED_GREEN;
        }
        break;
    }

    // To not go over the maximum value of 65535
    if (TB3R >= 60000)
        TB3R = 1;

    TB3CCR1 = TB3CCR1;
    TB3CCR2 = TB3CCR2;
    TB3CCR3 = TB3CCR3;

    TB1CCR0 += 10;      // Fade speed
}



