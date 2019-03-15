/*
 * GccApplication11.c
 *
 * Created: 2019-02-19 8:15:24 PM
 * Author : vaha
 */ 

#define F_CPU 1200000UL
#include <avr/eeprom.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define motor_signal 0

#define motor_switch 1

#define limiter_button1 5 //using reset pin as GPIO

#define limiter_button2 2 // limiter button....do i need it ????

#define open_signal 3 // to rotate motor forward

#define close_signal 4 // to rotate motor backwards

#define saved_position 1 //eeprom address to save

#define open_position 9

#define closed_position 18

#define forward 1

#define reverse 2

#define undefined 0

#define PINB_forward 8

#define PINB_reverse 16

#define SET_DDRB_OUTPUT() (DDRB |= _BV(DDB0) | _BV(DDB1))

#define SET_DDRB_INPUT() (DDRB &= ~(_BV(DDB0) | _BV(DDB1)))

#define SET_PORTB() (PORTB |= _BV(DDB2) | _BV(DDB3) | _BV(DDB4) | _BV(DDB5))

#define SET_GIMSK() (GIMSK |= _BV(PCIE))

#define SET_PCMSK() (PCMSK |= _BV(PCINT3) | _BV(PCINT4))

#define ADC_OFF() (ADCSRA &= ~_BV(ADEN))

#define SET_MOTOR_SWITCH_OFF() (PORTB &= ~_BV(motor_switch))

#define SET_MOTOR_SWITCH_ON() (PORTB |= _BV(motor_switch))

#define time_limit 16 //300ms / 19ms(OVF)

#define TOP 188 //top value for counter

void turn_motor(unsigned char _direction);

void sleep(void);

volatile unsigned char interrupted_signal;

int main(void){

 SET_PORTB(); // pins 2,3,4,5 input pullup enable

 SET_GIMSK(); //pin change interrupt enable

 SET_PCMSK(); // pin change on pins 3 and 4 active

 ADC_OFF(); // switch off ADC

 sleep();



    while (1) 
    {

		switch (interrupted_signal) {

			case (forward):
			turn_motor(forward);
			break;

			case (reverse):
			turn_motor(reverse);
			break;

			default:
			sleep();
			break;

		}
	}
}

ISR(PCINT0_vect) {

	if (!(PINB & PINB_forward)) {
    
    old_signal = forward;
    
    if (old_signal != interrupted_signal) interrupted_signal = old_signal;
    
  }
  else if (!(PINB & PINB_reverse)) {
    
    old_signal = reverse;
    
    if (old_signal != interrupted_signal) interrupted_signal = old_signal;
  
  }

}

//sleep mode
void sleep(void) {

	set_sleep_mode(SLEEP_MODE_PWR_DOWN);

	sleep_enable();                         // Sets the Sleep Enable bit in the MCUCR Register (SE BIT)

	//sleep_bod_disable(); ///disiabbled through fuses

	sei();                                  // Enable interrupts

	sleep_cpu();                            // sleep

	cli();                                  // Disable interrupts

	sleep_disable();                        // Clear SE bit

	sei();                                  // Enable interrupts
} // sleep

void turn_motor(unsigned char _direction) {
  
  SET_DDRB_OUTPUT(); // pin 0 and 1 output
  SET_MOTOR_SWITCH_ON(); // turn motor switch HIGH

  /*Start phase correct pwm */
  TCCR0A = _BV(WGM00) | _BV(COM0B1) | _BV(COM0B0); // mode5 , set on rising clear on falling
  TCCR0B = _BV(WGM02) | _BV(CS01) | _BV(CS00); //64 prescaler
  OCR0A = TOP;

  OCR0B = eeprom_read_byte((unsigned char *)saved_position);

  if (OCR0B > closed_position) OCR0B = closed_position;

  if (OCR0B < open_position) OCR0B = open_position;

  switch (_direction) {

    case forward:

      for (; OCR0B <= closed_position /*&& digitalRead(limiter_button)*/; OCR0B++ ) // goes from 60 degrees to 120 degrees

        _delay_ms(15);                              // waits 15 milliseconds for the servo to reach the position

      break;

    case reverse:

      for (; OCR0B >= open_position ; OCR0B--) // goes from 120 degrees to 60 degrees

        _delay_ms(15);                              // waits 15 milliseconds for the servo to reach the position

      break;

    default:
      break;
  }

  TCCR0B = 0; //timer/counter stopped

  SET_MOTOR_SWITCH_OFF(); // turn motor_switch LOW PORTB^=(1<<1);

  SET_DDRB_INPUT(); // pins 0 and 1 input DDRB^=3;

  eeprom_update_byte((unsigned char *)saved_position, OCR0B);

  return;

}
