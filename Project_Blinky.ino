/*

Project Blinky is a tiny, minimalist, configurable, and 
highly efficient LED flasher that will run from a single
coin cell battery or other 3V lithium source.

When the circuit is in sleep mode the current draw is only
7μA! With both LEDs on current draw is around 3.3mA. Given
that the LEDs are on for only 1.5% of the time this means 
average current draw is 58μA at 3V.

The primary power saving measures used to achieve this low
current draw are:
  - Using ATTiny85 with internal 1Mhz clock
  - Putting the CPU into deep sleep most of the time
  - Using the watchdog timer as an interrupt to wake the CPU
  - Disabling brownout detection (saves 25 µA)
  - Disabling the ADC (saves 335 µA)

Info on AVR power optimization: http://www.gammon.com.au/power

Expected battery life:
  - CR2320 (135mAh)  = 97 days
  - CR2032 (240mAh)  = 172 days
  - CR123A (1500mAh) = 3 years

                   ATTiny85
                  +---\/---+
          Led4 -  |1*     8|  VCC
          Led2 +  |2      7|  Led1 +
          Led2 -  |3      6|  Led1 -
             GND  |4      5|  Led3 +
                  +--------+

 */

#include <avr/sleep.h>
#include <avr/wdt.h>

const int ledPin1 = 3; // Package pin 7
const int ledPin2 = 2; // Package pin 2
const int ledPin3 = 0; // Package pin 5

int loop_count = 0;

void setup() {
  // Led sources
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  
  // Led drains
  pinMode(4, OUTPUT);
  pinMode(1, OUTPUT);
  digitalWrite(4, LOW);
  digitalWrite(1, LOW);
}

// Main loop happens once every 32ms when
// watchdog interrupt wakes up the CPU
void loop() {
  leds_on();
  
  enable_watchdog_32ms();
  sleep_until_watchdog_timeout();
  
  leds_off();
  
  enable_watchdog_2s();
  sleep_until_watchdog_timeout();
}

void leds_on() {
  digitalWrite(ledPin1, HIGH);
  digitalWrite(ledPin2, HIGH);
}

void leds_off() {
  digitalWrite(ledPin1, LOW);
  digitalWrite(ledPin2, LOW);
}

void enable_watchdog_32ms() {
  // set WDIE(watchdog interrupt enable), and 32ms delay
  WDTCR = bit (WDIE) | bit (WDP0);
  wdt_reset();
}

void enable_watchdog_2s() {
  // set WDIE(watchdog interrupt enable), and 2s delay
  WDTCR = bit (WDIE) | bit (WDP2) | bit (WDP1) | bit (WDP0);
  wdt_reset();
}

void sleep_until_watchdog_timeout() {
  ADCSRA = 0; // Disable ADC
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);  
  sleep_enable();
  sleep_bod_disable();
  sleep_cpu ();
}

// Interrupt from watchdog
ISR(WDT_vect) {
  // No-op
}
