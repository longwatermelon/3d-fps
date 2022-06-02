CC=gcc
CFLAGS=-std=gnu17 -ggdb -Wall -Wpedantic -Werror
LIBS=-lm -lSDL2 -lSDL2_ttf -lSDL2_mixer
INC=

CFLAGS+=$(FLAGS)
LIBS+=$(LIBRARIES)
INC+=$(INCLUDE)

SRC=$(wildcard src/*.c)
OBJS=$(addprefix obj/, $(SRC:.c=.o))

all:
	mkdir -p obj/src
	$(MAKE) rasterize

rasterize: $(OBJS)
	$(CC) $(CFLAGS) $(INC) $^ $(LIBS)

obj/src/%.o: src/%.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@ $(LIBS)

clean:
	-rm -rf obj rasterize

