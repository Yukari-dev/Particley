CC ?= gcc
CFLAGS ?= -O2 -g -Wall -Wextra -Wshadow -Wconversion
CFLAGS += -I. -Imodules
LDLIBS ?= -lSDL3 -lm
MODULE ?= doom-fire

COMMON_SRC := main.c src/render.c
MODULE_SRC := $(wildcard modules/$(MODULE)/*.c)

ifeq ($(MODULE_SRC),)
$(error unknown module '$(MODULE)'. Available: $(notdir $(wildcard modules/*)))
endif

SRC := $(COMMON_SRC) $(MODULE_SRC)
OBJ := $(SRC:.c=.o)
BIN := $(MODULE)

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $< -MMD -MP

-include $(OBJ:.o=.d)

run: $(BIN)
	./$(BIN)

list:
	@echo "Available modules: $(notdir $(wildcard modules/*))"

fclean:
	rm -f $(OBJ) $(OBJ:.o=.d)

clean: fclean
	rm -f $(BIN)

.PHONY: all list clean fclean run
