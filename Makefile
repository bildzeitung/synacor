# Makefile
#

CC=gcc
CFLAGS=-g -Wall -Ofast -Werror -Wextra -std=gnu99 -pedantic
OUTDIR=bin
LDFLAGS=-largp

.PHONY: clean all

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(OUTDIR)/vm

$(OUTDIR):
	mkdir $@ &> /dev/null || true

$(OUTDIR)/vm:: $(OUTDIR)

$(OUTDIR)/vm:: vm/vm.c vm/main.o
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	rm -f vm/*.o
	rm -fr $(OUTDIR)

