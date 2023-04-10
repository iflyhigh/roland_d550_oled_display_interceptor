# roland_d550_oled_display_interceptor
Arduino-based interceptor for Roland D-550 OLED display mod

OLED controller WS0010 found in many cheap OLED displays is almost compatible with typical HD44780 controller (and its nameless derivatives) but have one significant difference - by default (on poweron) WS0010 is initialized for right-to-left typing while HD44780 is initialized for left-to-right typing. Roland D-550 unlike, say, Yamahas of the same age, does not explicitly set display entry mode thus these cheap and always available OLED displays do not directly work in D-550 and do work fine with Yamahas like TX-802 or TX-81z. There are some sources that claim wrong timings to be the cause of this issue. Timings are fine in D-550, although it does not query display for command completion assuming its timings are just good enough (i.e., really slow - shortest 
strobe seen on E pin is 7.5us, i.e. 15 times longer than minimum required). R/W pin is grounded in D-550.

D-550 starts configuring display in ~330ms after powerup. My tests with Winstar OLED has shown that entry mode for WS0010 controller can be set even before the controller is configured for display size. So this interceptor just does two simple things:

 1. Upon powerup it sends command 0x6 which means "No shift, Left-to-Right" before D-550 starts display initialization.  ATMega 328p @16MHz takes ~70msec for booting, hence the delay(150) in setup() - this will execute code at  ~220msec after startup.
 
 2. Then it attaches interrupt to Arduino digital pin 3 that  is connected to E line of display control output of D-550 and waits for FALLING edge because both HD44780 and WS0010 process data bus while E is falling. ISR just copies inputs  to outputs not affecting data in any way. ISR code is short enough to perform OLED output before next ISR invocation that happens once in 60us.

This program assumes no Arfuino bootloader is present since it delays boot by ~1.5 seconds. So one needs to use ISP (in-system-programmer) or another Arduino with ArduinoISP sketch loaded and then use 'Sketch -> Upload using programmer' menu option.

Hope this will help saving you some bucks. Things may differ in other D-550 firmwares (unlikely). This interceptor should also work in D-50, but is not tested yet.

Input and output pins chosen for code simplification, not for comfortable PCB tracing. PCB design pending, mine is working OK on a prototyping board. See code comments for pin mapping.

You are OK to do whatever you want with this work. CC-BY 4.0

P.S.: much wisier approach whould be modifying D-550 FW, but given the fact that MCU does not talk to display directly but via a huge custom gate array... Good luck with this :)
