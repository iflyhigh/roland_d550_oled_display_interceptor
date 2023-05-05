# Roland D-550 OLED display interceptor
## Arduino-based interceptor for Roland D-550 OLED display mod

OLED controller WS0010 found in many cheap OLED displays is almost compatible with typical HD44780 controller (and its nameless derivatives) but many displays using this controller have one significant difference - by default (on poweron) they are initialized for right-to-left typing while LCDs with HD44780 are initialized for left-to-right typing. Roland D-550 unlike, say, Yamahas of the same age, does not explicitly set display entry mode thus these cheap and always available OLED displays do not directly work in D-550 and do work fine with Yamahas like TX-802 or TX-81z. There are some sources that claim wrong timings to be the cause of this issue. Timings are fine in D-550, although it does not query display for command completion assuming its timings are just good enough (i.e., really slow - shortest strobe seen on E pin is 7.5us, i.e. 15 times longer than minimum required). R/W pin is grounded in D-550.

D-550 starts configuring display in ~330ms after powerup. My tests with Winstar OLED has shown that entry mode for WS0010 controller can be set even before the controller is configured for display size. So this interceptor just does three simple things:

1. Upon powerup it sends command 0x6 which means "No shift, Left-to-Right" before D-550 starts display initialization. ATMega 328p @16MHz takes ~70msec for booting, hence the delay(150) in setup() - this will start executing the code at ~220msec after startup.

2. After some wait it clears the display. Clear takes up to 2 msec, so significant wait is introduced after this call to ensure display is cleared.

3. Then it attaches interrupt to Arduino digital pin 3 that is connected to E line of display control output of D-550 and waits for FALLING edge interrupt because both HD44780 and WS0010 process data bus while E is falling. ISR then just copies inputs to outputs not affecting data in any way. ISR code is short enough to perform OLED output before next ISR invocation that happens once in 60us.

This program assumes no Arfuino bootloader is present since it delays boot by ~1.5 seconds. So one needs to use ISP (in-system-programmer) or another Arduino with ArduinoISP sketch loaded and then use 'Sketch -> Upload using programmer' menu option.

Hope this will help saving you some bucks. Things may differ in other D-550 firmwares (unlikely), mine is 1.01. This interceptor should also work in D-50, but is not tested yet.

Input and output pins chosen for code simplification, not for comfortable PCB tracing. PCB design pending, mine is working OK on a prototyping board. See code comments for pin mapping. I've used Pro Mini board, you will need 5V 16MHz 328p version. Mine has wrong marking for pins D0 and D1 (D1 is actually in top left corner, and it is TX pin while my boards for some reason have pins called TX0 and RX1). You still need to heavily sand down and then isolate top parts of these cheap OLED display PCB to fit D-550 case.

2 protocol dumps are provided for reference, both captured with Saleae Logic 2. One is for original hardware, second one - with interceptor board installed (v1.0 code, no display clear command at that time).

You are OK to do whatever you want with this work. CC-BY 4.0.

P.S.: much wisier approach whould be modifying D-550 FW, but given the fact that MCU does not talk to display directly but via a huge custom gate array... Good luck with this :)
