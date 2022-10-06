CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -g
LDFLAGS = -lm -lncurses
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, build/%.o, $(SRC))
EXEC = build/Beasts

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

build/%.o: src/%.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(OBJ) $(EXEC)

.HANDY: clean all
