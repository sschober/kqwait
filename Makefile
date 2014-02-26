uname=$(strip $(shell uname))

ifeq "$(uname)" "Linux"
  INCLUDES=-I../libkqueue/include
  LIBS=-L../libkqueue/.libs -lkqueue -lpthread
  DEFINES=-DLINUX
endif

OPTS=$(INCLUDES) $(LIBS) $(DEFINES) -std=c99 -Wall -g

ALL: minimal

minimal: minimal.c
	cc $(OPTS) -o $@ $<
