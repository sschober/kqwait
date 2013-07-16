trg=kqwait
src=kqwait.c
git_version := $(shell git describe --abbrev=4 --dirty --always | sed "s;kqwait-;;")
uname=$(strip $(shell uname))

ifeq "$(uname)" "Linux"
  INCLUDES=-I/usr/include/kqueue
  LIBS=-lkqueue -lpthread
endif

OPTS=$(INCLUDES) $(LIBS) -std=c99 -Wall -g

.PHONY: clean

ALL: $(trg)

version.h:
	sed -e "s;%%VERSION%%;\"$(git_version)\";" version.h.tmpl > $@

dirinfo.o: dirinfo.c
	cc $(OPTS) -c -o $@ $<

$(trg): $(src) dirinfo.o version.h
	cc $(OPTS) -o $@ dirinfo.o $<

clean:
	rm -rf $(trg) $(trg).dSYM version.h

test: $(trg)
	prove -v t/*.pl
