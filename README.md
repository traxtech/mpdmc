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

Configuration is done with environment variables :
- MPDMC_MPD_HOST : MPD host (default: localhost)
- MPDMC_MPD_PORT : MPD port (default: 6600)
- MPDMC_MPD_PASSWORD : MPD password (default: empty)

This software is still in development. Use it at your own risks.
