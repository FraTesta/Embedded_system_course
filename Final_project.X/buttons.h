#ifndef XC_HEADER_BUTTONS_H
#define	XC_HEADER_BUTTONS_H

#define S6NOTPRESSED 0
#define S6PRESSED    1

// S5 button ISR
//void __attribute__((__interrupt__, __auto_psv__)) _INT0Interrupt();
// S6 buttons ISR
//void __attribute__((__interrupt__, __auto_psv__)) _INT1Interrupt();
// Debouncing timer ISR
//void __attribute__((__interrupt__, __auto_psv__)) _T3Interrupt();
// Debouncing timer
void tmr3_setup_period(int ms);

// Function enabling flag of buttons
void setButton();

#endif	/* XC_HEADER_BUTTONS_H */

