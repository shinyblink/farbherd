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

%: src/%.c $(DEP)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ src/$@.c

.PHONY:
install: $(BINS)
	mkdir -p $(DESTDIR)/bin
	mkdir -p $(DESTDIR)/include
	install $(BINS) $(DESTDIR)/bin
	install src/farbherd.h $(DESTDIR)/include

clean:
	rm -f $(BINS)
