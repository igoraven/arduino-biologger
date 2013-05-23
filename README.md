# arduino-biologger

## Overview

This project is a prototype of wearable data logger that measures different metrics of human body and environment. It is based on high-performance Arduino-compatible dev board Teensy++ 2.0 from PJRC and is fully compatible with modern Arduino boards and their derivatives. Source code is written in Arduino development environment.

## Scheme

![Fritzing scheme](https://raw.github.com/igoraven/arduino-biologger/master/biologger/biologger-fz.png "Fritzing scheme")

## Description

Biologger blocks:
* Controller board
    This version is based on PJRC Teensy++ 2.0, but it can be easily replaced by its smaller version - Teensy 2.0 - or any similiar board. If the overall size is not a concern, any modern Arduino - Uno, Ethernet, Mega, Mini - can be used. Main hardware requirement is to be able to communicate with all attached sensors, SD card, buttons and serial or parallel LCD.
* Sensors
    All sensors are attached to either analog or digital inputs and their list can be extended to register new metrics. All analog sensors have to be attached via voltage divider scheme to ensure 0 to 1023 ADC range. All sensors are attached to the Vcc with one wire and to the Ground through the resistor (10k for all sensors and 100k-300k for GSR); measure point is between sensor and resistor.
    Analog sensors:
    * GSR (Galvanic Skin Response) - is one of the main human's stress indicators. This sensor requires two metal (copper ar aluminium, for example) contact pads to be attached directly to skin, preferably to "sweaty" part of body, such as fingers. This sensor, in fact, is two electrodes being shorted through skin, but since the skin's resistance is very high and thus the current is extremely low, entire construction is totally harmless.
    * Body and environment temperature, which are calculated from thermistor resistance. This project uses 10k NTC thermistors with known B parameter equal to 4300.
    * Luminance is measured through photoresistor.
    * Sound pressure is an experimental feature; contrary to the scheme above, it is not only an electret microphone, but a small board with amplifier and filters. Since the voltage level from the mic is a wave, it has to be measured every few microseconds. Due to very high frequency of simple sound noise, this project's measurement can give you approximate environmental sound level value, but the correct way to measure it is to record level very frequent and to calculate the RMS of these recordings.
    Digital sensors:
    * DHT11 temperature and humidity sensor. This is the cheapest and simpliest of its kind, but it can measure somewhat accurate or at least factory-calibrated values of T and RH (relative humidity). This sensor can be easily replaced by 1-wire Dallas, more accurate DHT22 or any other digital sensor chip.
* Data storage
    Recorded data is being written to the microSD card in CSV format. Each launch of the recording process opens new file (board checks files with data??.csv pattern and creates file with first available number) and writes a header into it. Then, while in recording mode, data is captured each 500 msecs and is placed into buffer. This buffer is being written out to the file after its depletion or when the recording stops; such method reduces SD card wearing and increases the speed of write routine.
* Display and controls
    This project uses Grove Serial LCD from Seeed Studios, where most of measurements are being shown. Actually, any LCD, serial or parallel, can be used, or you can replace it with few LEDs that indicate the recording process and other runtime info. Three buttons are used to control the board: LCD backlight on/off (to save energy), Environment type 1/2 (useful for different surroundings), Recording start/stop. Buttons are connected through chip's internal pull-up resistors and their state is parsed with Bounce lib to filter undesirable on-off noise.
* Power
    This project is powered from USB external battery that is usually used to charge phones/e-readers/etc. To minimize the size of the entire project, Li-Ion battery can be used in common with 3.3V voltage controller soldered to the back of Teensy board (but keep in mind that the board's performance will be reduced).

## TODO
* Add more sensors (pulse, barometer, air quality, GPS)
* Change DHT11 to something more accurate
* Reduce the size
* Correct the sound pressure measurement

## Links
* [Teensy++ 2.0 USB Development Board](http://www.pjrc.com/teensy/index.html)
* [GSR measurement](http://open3.cc/wiki/doku.php?id=bio_feedback)
* [Seeed Studio Grove Serial LCD](http://www.seeedstudio.com/wiki/index.php?title=Grove_-_Serial_LCD)
* [DHT11 library](http://playground.arduino.cc/main/DHT11Lib)
* [Bounce library](http://playground.arduino.cc/code/bounce)
* [Thermistor temperature measurement](http://en.wikipedia.org/wiki/Thermistor#B_or_.CE.B2_parameter_equation)