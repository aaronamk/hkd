CC     = gcc
CFLAGS = -std=c11 -Wall -D_POSIX_C_SOURCE=200809L
PREFIX = /usr/local

all: hkd hkd-relayer

hkd:
	$(CC) $(CFLAGS) -o hkd hkd.c

hkd-relayer:
	$(CC) $(CFLAGS) -o hkd-relayer hkd-relayer.c

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f hkd ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/hkd
	cp -f hkd-relayer ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/hkd-relayer

uninstall:
	$(RM) -f ${DESTDIR}${PREFIX}/hkd ${DESTDIR}${PREFIX}/hkd-relayer

clean:
	$(RM) hkd hkd-relayer

.PHONY: all install uninstall clean
