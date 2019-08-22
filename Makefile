CFLAGS=-O3
LIBS=-lroot -lbe -ldevice
CC=g++
OBJS = Launcher.o USBDeskbarInfoWin.o USBDeskbarRoster.o USBDeskbarView.o USBPreferenceFile.o


all: build

build: $(OBJS)
	$(CC) $(CFLAGS) -o USBDeskbarView $^ $(LIBS)
	rc USBDeskbarView.rdef
	xres -o USBDeskbarView USBDeskbarView.rsrc
	mimeset -f USBDeskbarView

.cpp.o:
	$(CC) $(CFLAGS) -c $^

clean:
	rm -f USBDeskbarView $(OBJS)
