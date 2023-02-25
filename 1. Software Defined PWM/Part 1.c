/*
 * SoftwarePWMExample.c
 *
 *  Created on: Feb 18, 2023
 *      Author: Joshua Guillermo
 *
 *      This example controls the LED connected to Pin 1.0 by PWM. You can change
the DutyCycle Global variable to change the brightness of the LED. You should vary
this to see how the brightness can change.
 *      You can also change this in the Variables/Expressions tab in the debugger
to experiment with it as well.
 */
#include <msp430.h>
unsigned short DutyCycle = 5000;         // 50% duty cycle
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                    // Stop WDT

    // Configure GPIO
    P1DIR |= BIT0;          // Configure Pin1.0 as an output
    P1OUT &= ~BIT0;         // Clear Pin1.0

    P6DIR |= BIT6;          // Configure Pin6.6 as an output
    P6OUT &= ~BIT6;         // Clear Pin6.6

    P2DIR &= ~BIT3;                         // Configure Pin2.3 as an input
    P2OUT |= BIT3;                          // Configure P2.3 as pulled-up
    P2REN |= BIT3;                          // P2.3 pull-up register enable
    P2IES &= ~BIT3;                         // P2.3 Low --> High edge
    P2IE |= BIT3;                           // P2.3 interrupt enabled

    P4DIR &= ~BIT1;                         // Configure Pin4.1 as an input
    P4OUT |= BIT1;                          // Configure P4.1 as pulled-up
    P4REN |= BIT1;                          // P4.1 pull-up register enable
    P4IES &= ~BIT1;                         // P4.1 Low --> High edge
    P4IE |= BIT1;                           // P4.1 interrupt enabled

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    // Configure Timer_A
    TB0CTL = TBSSEL__SMCLK | MC__UP | TBIE;       // SMCLK, UP mode
    TB0CCTL1 |= CCIE;                             // Enable TB0CCR1 Interrupt
    TB0CCR1 = DutyCycle;                          // Set TB0CCR1 to the value to set the duty cycle
    TB0CCR0 = 10000;                               // Upper limit

    TB1CTL = TBSSEL__SMCLK | MC__UP | TBIE;       // SMCLK, UP mode
    TB1CCTL1 |= CCIE;                             // Enable TB1CCR1
    TB1CCR1 = DutyCycle;                          // Set TB1CCR1 to the value to set the duty cycle
    TB1CCR0 = 10000;                               // Upper Limit

    __bis_SR_register(LPM3_bits | GIE);           // Enter LPM3, enable interrupts
    __no_operation();                             // For debugger
}


// Port 2 interrupt service routine
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
    P2IFG &= ~BIT3;     // Set interrupt flag to 0

    if(TB0CCR1 >= 10000)                        // If greater than or equal to the upper limit
        {
            TB0CCR1 = 0;                        // 0% duty cycle
        }
        else
        {
            TB0CCR1 += 1000;                   // Add 10% duty cycle
        }
}

//Port 4 interrupt service routine (Resets the timer)
#pragma vector=PORT4_VECTOR
__interrupt void Port_4(void)
{
    P4IFG &= ~BIT1;     // Set interrupt flag to 0

    if(TB1CCR1 >= 10000)
    {
        TB1CCR1 = 0;
    }
    else
    {
        TB1CCR1 += 1000;
    }
}

// Timer0_B1 Interrupt Vector (TBIV) handler
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_B1_VECTOR
__interrupt void TIMER0_B1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_B1_VECTOR))) TIMER0_B1_ISR (void)
#else
#error Compiler not supported!
#endif
{
    switch(__even_in_range(TB0IV,TB0IV_TBIFG))
    {
        case TB0IV_NONE:
            break;                               // No interrupt
        case TB0IV_TBCCR1:
            P1OUT &= ~BIT0;
            break;                               // TB0CCR1 Set the pin to a 0
        case TB0IV_TBCCR2:
            break;                               // CCR2 not used
        case TB0IV_TBIFG:
            P1OUT |= BIT0;                       // overflow Set the pin to a 1
            break;
        default:
            break;
    }
}

// Timer1_B1 Interrupt Vector (TBIV) handler
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER1_B1_VECTOR
__interrupt void TIMER1_B1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER1_B1_VECTOR))) TIMER1_B1_ISR (void)
#else
#error Compiler not supported!
#endif
{
    switch(__even_in_range(TB1IV,TB1IV_TBIFG))
    {
        case TB1IV_NONE:
            break;                               // No interrupt
        case TB1IV_TBCCR1:
            P6OUT &= ~BIT6;
            break;                               // TB1CCR1 Set the pin to a 0
        case TB1IV_TBCCR2:
            break;                               // CCR2 not used
        case TB1IV_TBIFG:
            P6OUT |= BIT6;                       // overflow Set the pin to a 1
            break;
        default:
            break;
    }
}
