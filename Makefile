CC     = gcc
INCS   = -I/usr/include/libevdev-1.0
LIBS   = -L/usr/lib/ -levdev -pthread
CFLAGS = -std=c11 -Wall -D_POSIX_C_SOURCE=200809L -O3
PREFIX = /usr/local

all: hkd hk-relay hk-relay-plugin

hkd:
	$(CC) $(CFLAGS) -o hkd hkd.c

hk-relay:
	$(CC) $(CFLAGS) -o hk-relay hk-relay.c ${INCS} ${LIBS}

hk-relay-plugin:
	$(CC) $(CFLAGS) -o hk-relay-plugin hk-relay-plugin.c

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f hkd ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/hkd
	cp -f hk-relay ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/hk-relay
	cp -f hk-relay-plugin ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/hk-relay-plugin

uninstall:
	$(RM) ${DESTDIR}${PREFIX}/bin/hkd ${DESTDIR}${PREFIX}/bin/hk-relay ${DESTDIR}${PREFIX}/bin/hk-relay-plugin

clean:
	$(RM) hkd hk-relay hk-relay-plugin

.PHONY: all install uninstall clean
