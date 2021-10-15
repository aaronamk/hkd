CC     = gcc
INCS   = -I/usr/include/libevdev-1.0
LIBS   = -L/usr/lib/ -levdev -pthread
CFLAGS = -std=c11 -Wall -D_POSIX_C_SOURCE=200809L -O3
PREFIX = /usr/local

all: hkd hkd-relay

hkd:
	$(CC) $(CFLAGS) -o hkd hkd.c

hkd-relay:
	$(CC) $(CFLAGS) -o hkd-relay hkd-relay.c ${INCS} ${LIBS}

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f hkd ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/hkd
	cp -f hkd-relay ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/hkd-relay

uninstall:
	$(RM) ${DESTDIR}${PREFIX}/bin/hkd ${DESTDIR}${PREFIX}/bin/hkd-relay

clean:
	$(RM) hkd hkd-relay

.PHONY: all install uninstall clean
