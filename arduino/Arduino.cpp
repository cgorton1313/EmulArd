/* Copyright (C) 2013 Calvin Beck

  Permission is hereby granted, free of charge, to any person
  obtaining a copy of this software and associated documentation files
  (the "Software"), to deal in the Software without restriction,
  including without limitation the rights to use, copy, modify, merge,
  publish, distribute, sublicense, and/or sell copies of the Software,
  and to permit persons to whom the Software is furnished to do so,
  subject to the following conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
  ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

*/

#include "Arduino.h"
#include "commands.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <unistd.h>


/*
  Fake serial methods.
 */

void FakeSerial::begin(unsigned long speed) {
    /* Send the SERIAL_BEGIN command identifier */
    ARDUINO_COMMAND(SERIAL_BEGIN);

    /* Send port and baud rate arguments */
    ARDUINO_SEND(this->port_number);
    ARDUINO_SEND(speed);
}


size_t FakeSerial::write(uint8_t value) {
    /* SERIAL_WRITE command */
    ARDUINO_COMMAND(SERIAL_WRITE);

    /* Port number and value for arguments */
    ARDUINO_SEND(this->port_number);
    ARDUINO_SEND(value);

    return 1;
}


size_t FakeSerial::write(const char *str) {
    size_t length = strlen(str);

    return this->write((uint8_t *)str, length);
}


size_t FakeSerial::write(const uint8_t *buffer, size_t length) {
    for (int i = 0; i < length; ++i) {
        this->write(buffer[i]);
    }

    return length;
}


size_t FakeSerial::print(const char *str) {
    return this->write(str);
}


size_t FakeSerial::println(const char *str) {
    size_t sent_bytes = this->print(str);
    sent_bytes += this->println();

    return sent_bytes;
}


size_t FakeSerial::print(char value) {
    return this->write(value);
}


size_t FakeSerial::println(char value) {
    size_t sent_bytes = this->print(value);
    sent_bytes += this->println();

    return sent_bytes;
}


size_t FakeSerial::print(int value, int base) {
    char buffer[128];

    if (base == DEC) {
        snprintf(buffer, sizeof(buffer), "%d", value);
    }
    else {
        snprintf(buffer, sizeof(buffer), "%X", value);
    }

    return this->write(buffer);
}


size_t FakeSerial::print(int value) {
    return this->print(value, DEC);
}


size_t FakeSerial::println(int value, int base) {
    size_t sent_bytes = this->print(value, base);
    sent_bytes += this->println();

    return sent_bytes;
}


size_t FakeSerial::println(int value) {
    return this->println(value, DEC);
}


size_t FakeSerial::print(unsigned int value, int base) {
    char buffer[128];

    if (base == DEC) {
        snprintf(buffer, sizeof(buffer), "%u", value);
    }
    else {
        snprintf(buffer, sizeof(buffer), "%X", value);
    }

    return this->write(buffer);
}


size_t FakeSerial::print(unsigned int value) {
    return this->print(value, DEC);
}


size_t FakeSerial::println(unsigned int value, int base) {
    size_t sent_bytes = this->print(value, base);
    sent_bytes += this->println();

    return sent_bytes; 
}


size_t FakeSerial::println(unsigned int value) {
    return this->println(value, DEC);
}


size_t FakeSerial::print(long value, int base) {
    char buffer[128];

    if (base == DEC) {
        snprintf(buffer, sizeof(buffer), "%ld", value);
    }
    else {
        snprintf(buffer, sizeof(buffer), "%lX", value);
    }

    return this->write(buffer);
}


size_t FakeSerial::print(long value) {
    return this->print(value, DEC);
}


size_t FakeSerial::println(long value, int base) {
    size_t sent_bytes = this->print(value, base);
    sent_bytes += this->println();

    return sent_bytes;
}


size_t FakeSerial::println(long value) {
    return this->println(value, DEC);
}


size_t FakeSerial::print(unsigned long value, int base) {
    char buffer[128];

    if (base == DEC) {
        snprintf(buffer, sizeof(buffer), "%lu", value);
    }
    else {
        snprintf(buffer, sizeof(buffer), "%luX", value);
    }

    return this->write(buffer);
}

size_t FakeSerial::print(unsigned long value) {
    return this->print(value, DEC);
}


size_t FakeSerial::println(unsigned long value, int base) {
    size_t sent_bytes = this->print(value, base);
    sent_bytes += this->println();

    return sent_bytes;
}


size_t FakeSerial::println(unsigned long value) {
    return this->println(value, DEC);
}


size_t FakeSerial::println() {
    size_t sent_bytes = this->write('\r');
    sent_bytes += this->write('\n');

    return sent_bytes;
}


int FakeSerial::read() {
    /* Send the read command */
    ARDUINO_COMMAND(SERIAL_READ);

    /* Argument is the port number */
    ARDUINO_SEND(port_number);

    /* Return the result */
    return receive_int(STDIN_FILENO);
}


int FakeSerial::peek() {
    /* Send the read command */
    ARDUINO_COMMAND(SERIAL_PEEK);

    /* Argument is the port number */
    ARDUINO_SEND(port_number);

    /* Return the result */
    return receive_int(STDIN_FILENO);
}


int FakeSerial::available() {
    /* Send the read command */
    ARDUINO_COMMAND(SERIAL_AVAILABLE);

    /* Argument is the port number */
    ARDUINO_SEND(port_number);

    /* Return the result */
    return receive_int(STDIN_FILENO);
}


/*
  Implementation of the Arduino functions.
 */

void pinMode(uint8_t pin, uint8_t mode) {
    /* Send PIN_MODE command identifier */
    ARDUINO_COMMAND(PIN_MODE);

    /* Send the pin argument, and then the mode */
    ARDUINO_SEND(pin);
    ARDUINO_SEND(mode);
}


int digitalRead(uint8_t pin) {
    /* Send DIGITAL_READ command */
    ARDUINO_COMMAND(DIGITAL_READ);

    /* Send the pin number argument */
    ARDUINO_SEND(pin);

    /* Receive the integer result */
    return receive_int(STDIN_FILENO);
}


void digitalWrite(uint8_t pin, uint8_t value) {
    /* Send DIGITAL_WRITE command */
    ARDUINO_COMMAND(DIGITAL_WRITE);

    /* Write our pin, and value */
    ARDUINO_SEND(pin);
    ARDUINO_SEND(value);
}


int analogRead(uint8_t pin) {
    /* Send ANALOG_READ command */
    ARDUINO_COMMAND(ANALOG_READ);

    /* Send the pin number argument */
    ARDUINO_SEND(pin);

    /* Receive integer result */
    return receive_int(STDIN_FILENO);
}


void analogWrite(uint8_t pin, int value) {
    /* Send ANALOG_WRITE command */
    ARDUINO_COMMAND(ANALOG_WRITE);

    /* Write our pin and value */
    ARDUINO_SEND(pin);
    ARDUINO_SEND(value);
}


void randomSeed(unsigned int seed) {
    srand(seed);
}


long random(long max) {
    return rand() % max;
}


long random(long min, long max) {
    return (rand() % (max - min)) + min;
}


void delay(unsigned long milliseconds) {
    usleep(milliseconds * 1000);
}


unsigned long micros() {
    return clock() * 1000000 / CLOCKS_PER_SEC;
}


unsigned long millis() {
    return micros() / 1000;
}
