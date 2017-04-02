CC := gcc
CFLAGS := -Wall -Wextra -Werror -pedantic --std=gnu99 -g

SDL := $$(sdl-config --libs)
SDLFLAGS := $$(sdl-config --cflags)

OBJS := plot.o geom.o
INCLUDES := $(OBJS:.o=.h)

all: test

test: test.c $(OBJS) $(INCLUDES)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(SDLFLAGS) -o $@ $< $(OBJS) $(SDL) -lm

%.o: %.c %.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $(SDLFLAGS) -o $@ -c $<
