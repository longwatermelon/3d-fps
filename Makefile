CC=gcc
CFLAGS=-std=gnu17 -ggdb -Wall -Werror -pedantic
LIBS=-lm -lSDL2

SRC=$(wildcard src/*.c)
OBJS=$(addprefix obj/, $(SRC:.c=.o))

all:
	mkdir -p obj/src
	$(MAKE) fps

fps: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

obj/src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

clean:
	-rm -rf obj

