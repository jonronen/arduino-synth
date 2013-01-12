arduino-synth
=============

A very simple sound generator implemented using the Arduino Uno.

Sounds are generated using a timer with an interrupt frequency of about 16384 Hz.
Wave generation and effects are inspired by sfrx (http://www.drpetter.se/project_sfxr.html).
Further inspiration by the Arduino Octosynth (http://www.instructables.com/id/The-Arduino-OctoSynth/), Auduino (), and ... ().

The code is compiled with a Makefile that uses the AVR utils as well as some Arduino basic code.
I used a Makefile instead of the Arduino dev env because it creates a much faster code.
Otherwise I wouldn't have been able to keep up with the interrupt rate.
