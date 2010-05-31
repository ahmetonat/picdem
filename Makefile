# This file is part of fsusb_demo
#
# For Windows, please put usb.h and MingW version of libusb.a into
# the MingW include and lib directory respectively.
# For Linux only
LIBUSB = /usr/lib

OPTS=-Wall -D_GNU_SOURCE
#OBJS=fsusb_demo.o usb_func.o
OBJS=sub2_demo.o sub2usbfunc.o usblin.o

CFLAGS=$(OPTS) -I$(LIBUSB)/include
LDFLAGS=-L$(LIBUSB)/lib -lusb

# Needed for static linking under OS X:
# LDFLAGS=-lusb -lIOKit -framework CoreFoundation

all: sub2_demo  #fsusb_demo

sub2_demo: $(OBJS)  #fsusb_demo:
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

clean:
	-rm sub2_demo *.o
