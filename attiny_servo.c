/*
 * GccApplication11.c
 *
 * Created: 2019-02-19 8:15:24 PM
 * Author : vaha
 */ 

#define F_CPU 1200000UL
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define motor_signal PB1

#define motor_switch PB0

#define limiter_button PB2 // limiter button....do i need it ????

#define open_signal PB3 // to rotate motor reverse

#define close_signal PB4 // to rotate motor forward

#define saved_position 1 //eeprom address to save

#define open_position 179

#define closed_position 169

#define forward 1

#define reverse 2

#define SET_GIMSK() (GIMSK |= _BV(PCIE))

#define SET_GIMSK_OFF() (GIMSK&=~_BV(PCIE))

#define SET_PCMSK() (PCMSK |= _BV(PCINT3) | _BV(PCINT4))

#define ADC_OFF() (ADCSRA &= ~_BV(ADEN))

#define SET_MOTOR_SWITCH_OFF() (PORTB &= ~_BV(motor_switch))

#define SET_MOTOR_SWITCH_ON() (PORTB |= _BV(motor_switch))

//#define time_limit 16 //300ms / 19ms(OVF)

#define TOP 188 //top value for counter

void turn_motor(unsigned char _direction);

void sleep(void);

volatile unsigned char interrupted_signal;

volatile unsigned char current_state;

int main(void){

 DDRB |= _BV(motor_switch) | _BV(motor_signal);//set OUTPUT pins

  PORTB |= ~(_BV(motor_switch) | _BV(motor_signal));// output low

  PORTB |= _BV(limiter_button) | _BV(open_signal) | _BV(close_signal);// set pullups

  SET_GIMSK(); //pin change interrupt enable

  SET_PCMSK(); // pin change on pins 3 and 4 active

  ADC_OFF(); // switch off ADC

  sleep();



    while (1) 
    {

		switch (interrupted_signal) {

    case (forward):
      turn_motor(forward);
      current_state = forward;
      break;

    case (reverse):
      turn_motor(reverse);
      current_state = reverse;
      break;

    default:
      break;

  }

  sleep();
}

ISR(PCINT0_vect) {

	if (bit_is_clear(PINB, close_signal) && current_state != forward) interrupted_signal = forward;

  else if (bit_is_clear(PINB, open_signal) && current_state != reverse) interrupted_signal = reverse;

  else interrupted_signal = 0;

}

//sleep mode
void sleep(void) {

	void sleep(void) {

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  cli();

  sleep_enable();                         // Sets the Sleep Enable bit in the MCUCR Register (SE BIT)

  //sleep_bod_disable(); ///disiabbled through fuses

  sei();                                  // Enable interrupts

  sleep_cpu();                            // sleep                                 // Disable interrupts

  sleep_disable();                        // Clear SE bit

  sei();                                  // Enable interrupts
} // sleep
  
 void turn_motor(unsigned char _direction) {

  SET_GIMSK_OFF();

  SET_MOTOR_SWITCH_ON(); // turn motor switch HIGH

  /*Start phase correct pwm */
  TCCR0A = _BV(WGM00) | _BV(COM0B1) | _BV(COM0B0);// mode5 , set on rising clear on falling//transfer to setup

  TCCR0B = _BV(WGM02) | _BV(CS01) | _BV(CS00); //64 prescaler

  OCR0A = TOP;


  switch (_direction) {

    case forward:

      for (unsigned char i = 0; i <= 10 && bit_is_set(PINB,limiter_button); i++) {
        OCR0B = closed_position;
        _delay_ms(15);
      }
      break;

    case reverse:
      for (unsigned char i = 0; i <= 10; i++) {
        OCR0B = open_position;
        _delay_ms(15);  // goes from 120 degrees to 60 degrees
      }
      break;

    default:
      break;
  }

  SET_MOTOR_SWITCH_OFF(); // turn motor_switch LOW PORTB^=(1<<1);

  TCCR0B = 0; //timer/counter stopped

  TCCR0A = 0;

  SET_GIMSK();// enable pinchange global interrupt

  return;

}
