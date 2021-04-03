GBS8200 line doubling driver
============================

What it is
----------

This is a port of https://github.com/pulkomandy/gembascan for Wemos D1 mini (and surely other arduino boards). 

It allows to use the GBS8200 RGB to VGA converter in simple line doubling mode,
avoiding all the artifacts and problems caused by resampling and 50>60Hz
conversion.

The output is progressive scan VGA at 50Hz and 31.5KHz.

Hardware setup
--------------

Quite simple actually.

This was tested with GBS8200 V4.0 (2014.05.15) and V5.0 2015.01.15.

First of all, you need to disable the built-in controller of the GBS8200. This
is done by closing the P8 jumper.

Then, you need to conenct the Launchpad to the internal I2C bus on the P5 connector.
Note that you may need to solder pins to this connector as it is usually not
populated.

The wiring is as follows:

Wemos > GBS8200

* D1 > P5.SCL
* GND > P5.GND
* D2 > P5.SDA

Leave P5.VCC unconnected. 
You can connect P9.+ to Wemos 5V ONLY if you are powering GBS with 5V. In other case just connect Wemos to a USB charger.


How to build
------------

You will need to install Arduino IDE and the repository for ESP8266.
Then, select your board (Wemos D1 Mini or whatever)

For a detailed guide you can check https://randomnerdtutorials.com/how-to-install-esp8266-board-arduino-ide/

Once installed, just compile and flash it.

How to use it
-------------

Also quite simple.

First connect the video inputs and outputs. You may need a SCART to VGA adapter
for the input (or whatever connector you want to use). Plug the power supply to
the GBS8200. Nothing happens at this point, this is expected. Connect power to
the Launchpad. It automatically sets the chip up and starts running.

A serial terminal is available on the Launchpad debug USB port. It allows you
to peek and poke the video controller registers for adjustments and experimentation.
Later, more high-level commands will be added.

You can also reflash the Stellaris or press the reset button at any time. It
will reprogram the scandoubler again, which is useful if something goes wrong
or if you want to quickly test changes to the code.

Compatibility
-------------

This was confirmed working with the following input sources:
- Amstrad 6128Plus
- Amstrad CPC
- Thomson TO8

And the following display hardware:
- No name "TFT color monitor" with VGA input, MST703 based.
- ACER video projector at Forever partyplace.
- "Linetech" brand TFT TV, manufactured by Vestel.

Known problems
--------------

The tolerance on off video timings is currently quite bad (far from what a
standard TV would accept). This creates problems with some Amstrad games using
hardware scrolling tricks (seen on Relentless for example), and also with ZX
spectrum ULA-128 timings. Ways to improve this are being investigated.


Credits
-------


Code for this implementation by issalig

Code is based on Launchpad code by PulkoMandy/SHR. https://github.com/pulkomandy/gembascan

Original idea and experiments with Raspberry Pi by dooklink (https://github.com/dooklink/gbs-control)

Digispark Pro C code and register value list converter tool by mybook4 (http://shmups.system11.org/viewtopic.php?f=6&t=52172)


Appendices
==========

SPECTRUM TIMINGS
----------------

Thanks to mike/zeroteam, the secrets of ZX spectrum video timings have been
revealed:

* ULA 48k: 312 lines, 64us/line, 19968us/frame, 50.080128Hz (Spectrum 48K)
* ULA 1  : 314 lines, 64us/line, 20096us/frame, 49.761144Hz (Didaktik)
* ULA-128: 311 lines, 64.281485us/line, 199991.5us/frame, 50.021153Hz (128k/+2/+3)

The 48k ULA is exactly identical to Amstrad CPC/Thomson timings and should not
be a problem. However, for some reason the timings for the other variants are
a bit off, and this seems to confuse the GBS converter (but also a lot of other
devices). Symptom: the first frame (boot screen) will be displayed fine, but then the screen never gets updated.

Datasheets
----------

A set of datasheets was assembled by dooklink. It contains details on most of the chips used on the GBS8200.
http://shmups.system11.org/viewtopic.php?f=6&t=52172
