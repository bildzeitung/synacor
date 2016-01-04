# Makefile
#

CC=gcc
CFLAGS=-g -Wall -Ofast -Werror -Wextra -std=gnu99 -pedantic
OUTDIR=bin
LDFLAGS=-largp

.PHONY: clean all

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(OUTDIR)/vm $(OUTDIR)/ackermann

$(OUTDIR):
	mkdir $@ &> /dev/null || true

$(OUTDIR)/vm $(OUTDIR)/ackermann:: $(OUTDIR)

$(OUTDIR)/vm:: vm/vm.o vm/main.o
	$(CC) $(LDFLAGS) -o $@ $^

$(OUTDIR)/ackermann:: vm/ackermann.o
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	rm -f vm/*.o
	rm -fr $(OUTDIR)

