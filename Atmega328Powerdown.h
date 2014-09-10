/****************************************************************

This file is part of the Arduino powerdwon library at http://github.com/unchiujar/arduino_powersave

    Powerdown is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    powersave is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with powersave.  If not, see <http://www.gnu.org/licenses/>.

An example on how to put the Arduino to sleep for an arbitrary period
of time. To wake up after a preset time the Watchdog timer is used.
The watchdog can be set to wake the system after a reset interval.
The intervals are preset (see the chart below or the Watchdog ATmega
technical specs) and depend on the voltage. To sleep for longer periods
of times than the longest preset period (2.2s at 3V) a number of sleep
cycles is calculated and the system is put to sleep immediately after
waking up until all the sleep cycles are completed.

This example uses code from

 * Watchdog Sleep Example
 * Demonstrate the Watchdog and Sleep Functions
 * Photoresistor on analog0 Piezo Speaker on pin 10
 *
 * KHM 2008 / Lab3/  Martin Nawrath nawrath@khm.de
 * Kunsthochschule fuer Medien Koeln
 * Academy of Media Arts Cologne


http://interface.khm.de/index.php/lab/experiments/sleep_watchdog_battery/

*****************************************************************/

#ifndef Atmega328Powerdown_h
#define Atmega328Powerdown_h

#include "Arduino.h"
#include "Powerdown.h"

class Atmega328Powerdown: public Powerdown
{
   public:
      Atmega328Powerdown(voltage_t voltage);
      void save(int interval);
      boolean awake();
   private:
      void calculateWaitPeriods5V(unsigned long sleep);
      void calculateWaitPeriods3V(unsigned long sleep);
      void sleep3V(unsigned long sleep);
      void sleep5V(unsigned long sleep);
      void doSleep();
      void system_sleep();
      void setup_watchdog(int ii);
      voltage_t _voltage;
      /*
       From ATMega328 Watchdog specs

       Number of WDT     Typical Time-out Typical Time-out
       WDP2 WDP1 WDP0 Oscillator Cycles    at VCC = 3.0V    at VCC = 5.0V
       0    0    0    0        16K (16,384)         17.1 ms          16.3 ms
       1    0    0    1        32K (32,768)         34.3 ms          32.5 ms
       2    0    1    0        64K (65,536)         68.5 ms           65 ms
       3    0    1    1       128K (131,072)         0.14 s           0.13 s
       4    1    0    0       256K (262,144)         0.27 s           0.26 s
       5    1    0    1       512K (524,288)         0.55 s           0.52 s
       6    1    1    0     1,024K (1,048,576)       1.1 s            1.0 s
       7    1    1    1     2,048K (2,097,152)       2.2 s            2.1 s
      */
      unsigned long wait_periods[8];

};

#endif

