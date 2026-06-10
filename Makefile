COMP ?= gcc
CFLAGS ?= -O3 -fPIC
LDFLAGS ?= -shared

all: libfastcalc.so

libfastcalc.so: fast_calc.c

	$(COMP) $(CFLAGS) $(LDFLAGS) -o $@ $<

clean:
	rm -f libfastcalc.so
