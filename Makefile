CC = gcc

CFLAGS = -Wall -pedantic -std=c11

DEBUG ?= 0
ifeq ($(DEBUG), 1)
	CFLAGS += -g
else
	CFLAGS += -O3
endif

LDFLAGS = -lm -lncurses -lpthread

SRC_DIR = src
BUILD_DIR = build

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC))
EXEC = $(BUILD_DIR)/Beasts

all: $(BUILD_DIR) $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

build/%.o: src/%.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -rf $(BUILD_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

run: $(BUILD_DIR) $(EXEC)
	./$(EXEC)

.PHONY: $(BUILD_DIR) clean run
