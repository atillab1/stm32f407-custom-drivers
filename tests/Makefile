# Host-side unit tests for the hardware-independent drivers.
#   make -C tests         build and run all tests
#   make -C tests clean   remove build output

CC      ?= cc
CFLAGS  ?= -std=c11 -O2 -Wall -Wextra -Wshadow -Werror
DRIVERS := ../drivers
BUILD   := build

TESTS := $(BUILD)/test_circular_buffer

all: $(TESTS)
	@for t in $(TESTS); do ./$$t || exit 1; done

$(BUILD)/test_circular_buffer: test_circular_buffer.c $(DRIVERS)/circular_buffer/circular_buffer.c $(DRIVERS)/circular_buffer/circular_buffer.h
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -I$(DRIVERS)/circular_buffer $(filter %.c,$^) -o $@

clean:
	rm -rf $(BUILD)

.PHONY: all clean
