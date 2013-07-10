trg=kqwait
src=kqwait.c

uname=$(strip $(shell uname))

ifeq "$(uname)" "Linux"
  INCLUDES=-I/usr/include/kqueue
  LIBS=-lkqueue -lpthread
endif

OPTS=$(INCLUDES) $(LIBS) -std=c99 -Wall -g

.PHONY: clean

ALL: $(trg)

dirinfo.o: dirinfo.c
	cc $(OPTS) -c -o $@ $<

$(trg): $(src) dirinfo.o
	cc $(OPTS) -o $@ dirinfo.o $<

clean:
	rm -rf $(trg) $(trg).dSYM
