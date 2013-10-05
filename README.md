mpdmc
=====

This is mpdmc, MPD Meta Cast.

This software is a MPD client that updates an USB LCD display, the PicoLCD graphics (256x64 resolution). It does not use any third party servers like lcdproc or lcd4linux.

Compilation and usage:
- make dep
- make
- ./mpdmc

Dependencies:
- libmpd (package libmpd-dev on Ubuntu 12.04)
- libusb (package libusb-1.0-0-dev on Ubuntu 12.04)
- cairo (package libcairo2-dev on Ubuntu 12.04)

This software is still in development. Use it at your own risks. It is not configurable and it assumes a MPD server on localhost, port 6600 and no password.
