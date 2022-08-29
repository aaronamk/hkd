# Author: aaronamk

CC     = gcc
INCS   = -I/usr/include/libevdev-1.0
LIBS   = -L/usr/lib/ -levdev -pthread
CFLAGS = -std=c11 -Wall -D_POSIX_C_SOURCE=200809L -O3
PREFIX = /usr/local

hkd:
	$(CC) $(CFLAGS) -o hkd hkd.c ${INCS} ${LIBS}

install: hkd
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f hkd ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/hkd

uninstall:
	$(RM) ${DESTDIR}${PREFIX}/bin/hkd

clean:
	$(RM) hkd

.PHONY: all install uninstall clean
