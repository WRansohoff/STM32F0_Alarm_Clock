# Overview

Firmware for an alarm clock.

That's all. It's just a barebones alarm clock. It uses an SSD1306 monochrome OLED display for displaying menus/options, four 7-segment LEDs driven by four 74HC595 shift registers for displaying the time, a DS3231 RealTime Clock module, and an STM32F030F4 or STM32F031F6 as a core microcontroller.

The STM32F03x chips do technically have an RTC onboard, but 3 things:

1. I don't understand the STM32's RTC peripheral and it broke half the time when I tried to use it for PRNG, and I only have like a week.

2. I'm using the TSSOP-20 versions of the pin, and they do not have pins for a low-frequency 32.768KHz oscillator. If I use the "about 40KHz" onboard oscillator the clock will be wildly inaccurate for 'alarm clock' duty, and if I use the high-frequency 48MHz oscillator I'd have to mess with the timing settings which I also don't understand.

3. The STM32 has no onboard EEPROM, so if I wanted to store an alarm between power-offs, I'd need to reserve an entire page of Flash memory - that's 1KB out of 16KB or 32KB total. For like, one or two bytes of data. Flash isn't ideal for storing nonvolatile config values. The DS3231 has two R/W alarm registers, so I don't even need one of the popular 'ZS-042' modules which include an EEPROM chip.

So...I spent $5 on a breakout board instead, sue me.
