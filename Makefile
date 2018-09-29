# farbherd Makefile
CC ?= cc
CFLAGS ?= -O2
CPPLAGS += -pedantic -Wall -Wextra

# Don't change after here.
# Or do. I am not your mom.
BINS=fhinfo fhpack fhunpack
DEP=src/farbherd.h

all: $(BINS)

fhinfo: src/fhinfo.c $(DEP)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ src/$@.c
fhpack: src/fhpack.c $(DEP)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ src/$@.c
fhunpack: src/fhunpack.c $(DEP)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ src/$@.c

clean:
	rm -f $(BINS)
