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

#ifndef Powerdown_h
#define Powerdown_h

#include "Arduino.h"

typedef enum { VOLTAGE_3V, VOLTAGE_5V } voltage_t;
typedef enum { ATMEGA328 } cpu_t;

class Powerdown
{
   public:
      // Factory method
      static Powerdown* make(cpu_t cpu, voltage_t voltage);
      virtual void save(int interval);
};

#endif

