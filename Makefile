# farbherd Makefile
CC ?= cc
CFLAGS ?= -O2
CPPLAGS += -pedantic -Wall -Wextra

DESTDIR ?= /usr/local

# Don't change after here.
# Or do. I am not your mom.
BINS=fhinfo fhpack fhunpack fhwrap
DEP=src/farbherd.h

all: $(BINS)

$(BINS): $(DEP)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ src/$@.c

.PHONY:
install: $(BINS)
	mkdir -p $(DESTDIR)/bin
	install $(BINS) $(DESTDIR)/bin
clean:
	rm -f $(BINS)
