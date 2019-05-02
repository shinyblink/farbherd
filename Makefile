# farbherd Makefile
CC ?= cc
CFLAGS ?= -O2
CPPFLAGS += -pedantic -Wall -Wextra

PREFIX ?= /usr/local
DESTDIR ?= /

# Don't change after here.
# Or do. I am not your mom.
BINS=fhinfo fhpack fhunpack fhwrap
DEP=src/farbherd.h

all: $(BINS)

fhinfo: src/fhinfo.c $(DEP)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ src/$@.c
fhpack: src/fhpack.c $(DEP)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ src/$@.c
fhunpack: src/fhunpack.c $(DEP)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ src/$@.c
fhwrap: src/fhwrap.c $(DEP)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ src/$@.c


.PHONY: install uninstall
install: $(BINS)
	install -d $(DESTDIR)/$(PREFIX)/bin
	install -d $(DESTDIR)/$(PREFIX)/include
	install $(BINS) $(DESTDIR)/$(PREFIX)/bin
	install src/farbherd.h $(DESTDIR)/$(PREFIX)/include

uninstall:
	cd $(DESTDIR)/$(PREFIX)/bin && rm -f $(BINS)
	rm -f $(DESTDIR)/$(PREFIX)/include/farbherd.h

clean:
	rm -f $(BINS)
