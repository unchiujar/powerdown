/****************************************************************
 * 
 * This file is part of the Arduino powerdwon library at http://github.com/unchiujar/arduino_powersave
 * 
 *    Powerdown is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 * 
 *    powersave is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 * 
 *    You should have received a copy of the GNU General Public License
 *    along with powersave.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * An example on how to put the Arduino to sleep for an arbitrary period
 * of time. To wake up after a preset time the Watchdog timer is used.
 * The watchdog can be set to wake the system after a reset interval.
 * The intervals are preset (see the chart below or the Watchdog ATmega
 * technical specs) and depend on the voltage. To sleep for longer periods
 * of times than the longest preset period (2.2s at 3V) a number of sleep
 * cycles is calculated and the system is put to sleep immediately after
 * waking up until all the sleep cycles are completed.
 * 
 * This example uses code from
 * 
 * Watchdog Sleep Example
 * Demonstrate the Watchdog and Sleep Functions
 * Photoresistor on analog0 Piezo Speaker on pin 10
 *
 * KHM 2008 / Lab3/  Martin Nawrath nawrath@khm.de
 * Kunsthochschule fuer Medien Koeln
 * Academy of Media Arts Cologne
 * 
 * 
 * http://interface.khm.de/index.php/lab/experiments/sleep_watchdog_battery/
 * 
 *****************************************************************/

#include <avr/sleep.h>
#include <avr/wdt.h>
#include "Arduino.h"
#include "Powerdown.h"

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

volatile boolean awake_flag = 1;
Powerdown::Powerdown(cpu_t cpu, voltage_t voltage)
{
  _cpu = cpu;
  _voltage = voltage;
  // CPU Sleep Modes
  // SM2 SM1 SM0 Sleep Mode
  // 0    0  0 Idle
  // 0    0  1 ADC Noise Reduction
  // 0    1  0 Power-down
  // 0    1  1 Power-save
  // 1    0  0 Reserved
  // 1    0  1 Reserved
  // 1    1  0 Standby(1)
  
  cbi( SMCR, SE );     // sleep enable, power down mode
  cbi( SMCR, SM0 );    // power down mode
  sbi( SMCR, SM1 );    // power down mode
  cbi( SMCR, SM2 );    // power down mode
  awake_flag=1;
}

void Powerdown::save(int interval)
{
  awake_flag = 0;
  sleep3V(interval);   
}

boolean Powerdown::awake()
{
  return awake_flag;
}
/*
 *  Calculates how many times the watchdog should
 *  be set for each sleep state. It goes through progressively
 *  shorter wait times until waking up. Since the
 *  internal oscillator runs differently for 5V and 3V
 *  the sleep times are calculated differently for 3V and 5V
 *  See the above chart and the Watchdog Timer and Minimizing Power
 *  consumption in the ATMega328 technical specifications.
 */
void Powerdown::calculateWaitPeriods5V(unsigned long sleep) {
  wait_periods[7] = sleep / 2100;
  sleep = sleep - wait_periods[7] * 2100;
  wait_periods[6] = sleep / 1000;
  sleep = sleep - wait_periods[6] * 1000;
  wait_periods[5] = sleep / 520;
  sleep = sleep - wait_periods[5] * 520;
  wait_periods[4] = sleep / 260;
  sleep = sleep - wait_periods[4] * 260;
  wait_periods[3] = sleep / 130;
  sleep = sleep - wait_periods[3] * 130;
  wait_periods[2] = sleep / 65;
  sleep = sleep - wait_periods[2] * 65;
  wait_periods[1] = sleep / 32;
  sleep = sleep - wait_periods[1] * 32;
  wait_periods[0] = sleep / 16;
}
void Powerdown::calculateWaitPeriods3V(unsigned long sleep) {
  wait_periods[7] = sleep / 2200;
  sleep = sleep - wait_periods[7] * 2200;
  wait_periods[6] = sleep / 1100;
  sleep = sleep - wait_periods[6] * 1100;
  wait_periods[5] = sleep / 550;
  sleep = sleep - wait_periods[5] * 550;
  wait_periods[4] = sleep / 270;
  sleep = sleep - wait_periods[4] * 270;
  wait_periods[3] = sleep / 140;
  sleep = sleep - wait_periods[3] * 140;
  wait_periods[2] = sleep / 68;
  sleep = sleep - wait_periods[2] * 68;
  wait_periods[1] = sleep / 34;
  sleep = sleep - wait_periods[1] * 34;
  wait_periods[0] = sleep / 17;
}

/*
 * Puts the system to sleep, the watchdog
 * wakes the system which is then put to sleep
 * again immediately until it cycles through 
 * all the values in wait_periods. 
 */
void Powerdown::doSleep(){
  //sleep routine
  for (byte i=7;i>0;i--){
    //set sleep time
    setup_watchdog(i);
    //sleep for the number of times
    for (unsigned long j=0;j<wait_periods[i];j++){
      system_sleep();
    }
  }
}

/*
 * Helper method to put the Arduino to sleep for
 * sleep milliseconds when powered at 3V.
 */
void Powerdown::sleep3V(unsigned long sleep) {
  calculateWaitPeriods3V(sleep);
  doSleep();
}

/*
 * Helper method to put the Arduino to sleep for
 * sleep milliseconds when powered at 5V.
 */
void Powerdown::sleep5V(unsigned long sleep) {
  calculateWaitPeriods5V(sleep);
  doSleep();
}

//****************************************************************
// set system into the sleep state
// system wakes up when watchdog is timed out
void Powerdown::system_sleep() {
  
  cbi(ADCSRA, ADEN);                   // switch Analog to Digitalconverter OFF
  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  sleep_enable();
  
  sleep_mode();                        // System sleeps here
  
  sleep_disable();                     // System continues execution here when watchdog timed out
  sbi(ADCSRA, ADEN);                   // switch Analog to Digitalconverter ON
  
}

//****************************************************************
/*
 * See the chart at the top for sleep times
 */
void Powerdown::setup_watchdog(int ii) {
  
  byte bb;
  int ww;
  if (ii > 9 ) ii = 9;
  bb = ii & 7;
  if (ii > 7) bb |= (1 << 5);
  bb |= (1 << WDCE);
  ww = bb;
  
  MCUSR &= ~(1 << WDRF);
  // start timed sequence
  WDTCSR |= (1 << WDCE) | (1 << WDE);
  // set new watchdog timeout value
  WDTCSR = bb;
  WDTCSR |= _BV(WDIE);
  
}
//****************************************************************
// Watchdog Interrupt Service / is executed when  watchdog timed out
ISR(WDT_vect) {
  awake_flag = 1; // set global flag
}

