# Quiet (set to @ for a quite compile)
#Q	?= @
Q	?=

# Build Tools
CC 	:= gcc
CFLAGS := -I. -O2 -Wall -funroll-loops -ffast-math $(shell pkg-config --cflags libmpd)
LD := gcc
LDFLAGS := -O2 -Wall

APP_OBJECTS = app.o dataset.o
APP_LIBS = $(shell pkg-config --libs libmpd)
APP_BIN = mpdmc

# PicoLCD module
CFLAGS += -DWITH_PICOLCD $(shell pkg-config --cflags cairo-xlib libusb)
APP_OBJECTS += picolcd.o
APP_LIBS += $(shell pkg-config --libs cairo-xlib libusb)

.PHONY: all clean dep load_default

all: Makefile $(APP_BIN)

dep:
	@echo DEP $@
	$(Q)for i in *.c; do $(CC) -MM $(CFLAGS) "$${i}" ; done > Makefile.dep

-include Makefile.dep

$(APP_BIN): $(APP_OBJECTS)
	@echo LD $@
	$(Q)$(LD) -o $(APP_BIN) $(APP_OBJECTS) $(APP_LIBS) $(LDFLAGS)

%.o: %.c
	@echo GCC $<
	$(Q)$(CC) -c $(CFLAGS) $<

clean:
	@echo Cleaning...
	$(Q)rm -vf *.o *.so $(APP_BIN) Makefile.dep

run: all
	MPDCAST_MPD_HOSTNAME=localhost MPDCAST_MPD_PORT=6600 ./mpdcli

install: all
	@echo Installing...
	$(Q)install -m 644 $(APP_BIN) ${DESTDIR}/usr/bin/

uninstall:
	@echo Un-installing...
	$(Q)rm ${DESTDIR}/usr/bin/$(APP_BIN)
	
