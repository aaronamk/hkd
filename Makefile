CC     = gcc
INCS   = -I/usr/include/libevdev-1.0
CFLAGS = -std=c11 -Wall -D_POSIX_C_SOURCE=200809L -O3
PREFIX = /usr/local

all: hkd hkd-relayer

hkd:
	$(CC) $(CFLAGS) -o hkd hkd.c

hkd-relayer:
	$(CC) $(CFLAGS) -o hkd-relayer hkd-relayer.c ${INCS}

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f hkd ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/hkd
	cp -f hkd-relayer ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/hkd-relayer

uninstall:
	$(RM) ${DESTDIR}${PREFIX}/bin/hkd ${DESTDIR}${PREFIX}/bin/hkd-relayer

clean:
	$(RM) hkd hkd-relayer

.PHONY: all install uninstall clean
