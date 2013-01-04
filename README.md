arduino-synth
=============

A very simple sound generator implemented using the Arduino Uno.

Sounds are generated using a timer with an interrupt frequency of about 16384 Hz.
Sound generation is inspired by sfrx (http://www.drpetter.se/project_sfxr.html).

The code is compiled with a Makefile that uses the AVR utils as well as some Arduino basic code.
I used a Makefile instead of the Arduino dev env because it creates a much faster code.
Otherwise I wouldn't have been able to keep up with the interrupt rate.
