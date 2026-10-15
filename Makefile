CC       ?= cc
CSTD     ?= -std=c11
WARN     ?= -Wall -Wextra -Wpedantic
CFLAGS   ?= $(CSTD) $(WARN) -O2 -g
CPPFLAGS ?= -Isrc

BUILD := build
BIN   := $(BUILD)/qcc
SRC   := $(wildcard src/*.c src/*/*.c)
OBJ   := $(patsubst src/%.c,$(BUILD)/obj/%.o,$(SRC))
DEP   := $(OBJ:.o=.d)
SANFLAGS := $(CSTD) $(WARN) -O1 -g -fsanitize=address,undefined

.PHONY: all clean test sanitize

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD)/obj/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

-include $(DEP)

test: $(BIN)
	@./tests/run.sh

sanitize:
	@$(MAKE) clean
	@$(MAKE) CFLAGS="$(SANFLAGS)" all

clean:
	rm -rf $(BUILD)
