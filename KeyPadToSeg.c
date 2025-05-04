#include <mega16.h>
#include <stdio.h> 
#include <delay.h>

unsigned char SS_PATTERN[16] = {
    63, 6, 91, 79, 102, 109, 125, 7,
    127, 111, 119, 124, 57, 94, 121, 113
};

unsigned char counter = 0;
unsigned char SS_VALUES[4] = {0, 0, 0, 0};

interrupt [TIM1_COMPA] void timer1_compare_isr(void) {
    PORTA = (1 << (counter + 4));
    PORTB = SS_PATTERN[SS_VALUES[counter]];
    counter = (counter + 1) & 0b11;
}

void main(void) {

    unsigned char temp, key_counter = 0;

    DDRA = 0b11110000; // Upper 4 bits for segment select
    DDRB = 0xFF;       // PORTB as output for segment data

    PORTA = 0x00;
    PORTB = 0x00;

    DDRD = 0b00001111; // Lower nibble as output (columns)
    PORTD = 0x00;

    delay_ms(5); 

    OCR1A = 3500;
    TCCR1B = (1 << WGM12) | (1 << CS10);
    TIMSK |= (1 << OCIE1A);

    #asm("sei") // Enable interrupts

    while (1) {
        PORTD = (1 << key_counter);         // Set current row
        delay_us(1);                        // Small settle time
        temp = (PIND >> 4) & 0x0F;          // Read columns
        SS_VALUES[key_counter] = temp;      // Store pressed key
        key_counter = (key_counter + 1) & 0b11; // Next row
    }
}
