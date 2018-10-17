# farbherd Makefile
CC ?= cc
CFLAGS ?= -O2
CPPFLAGS += -pedantic -Wall -Wextra

DESTDIR ?= /usr/local

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
	mkdir -p $(DESTDIR)/bin
	mkdir -p $(DESTDIR)/include
	install $(BINS) $(DESTDIR)/bin
	install src/farbherd.h $(DESTDIR)/include

uninstall:
	cd $(DESTDIR)/bin && rm -f $(BINS)
	rm -f $(DESTDIR)/include/farbherd.h

clean:
	rm -f $(BINS)
