CC := gcc
CFLAGS := -Wall -Wextra -Werror -pedantic --std=gnu99 -g

SDL := $$(sdl-config --libs)
SDLFLAGS := $$(sdl-config --cflags)

all: test

test: test.c plot.h plot.o
	$(CC) $(CFLAGS) $(CPPFLAGS) $(SDLFLAGS) -o $@ $< plot.o $(SDL) -lm

%.o: %.c %.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $(SDLFLAGS) -o $@ -c $<
