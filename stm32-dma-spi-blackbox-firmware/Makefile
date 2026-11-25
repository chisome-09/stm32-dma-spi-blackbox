CC      ?= gcc
SRC_DIR ?= ..
INC     ?= -I $(SRC_DIR)
BUILD   := build
CFLAGS  := -std=gnu11 -Wall -Wextra -Wno-pointer-to-int-cast -g -I fake $(INC)
SRCS    := test_dma.c $(SRC_DIR)/dma.c

.PHONY: all test san cov clean

all: test

test: $(BUILD)/t_O0 $(BUILD)/t_O2
	./$(BUILD)/t_O0
	./$(BUILD)/t_O2

san: $(BUILD)/t_san
	./$(BUILD)/t_san

cov: $(SRCS) | $(BUILD)
	$(CC) $(CFLAGS) -O0 --coverage $(SRCS) -o $(BUILD)/t_cov
	./$(BUILD)/t_cov
	gcov -b $(BUILD)/t_cov-dma.gcda

$(BUILD)/t_O0: $(SRCS) | $(BUILD)
	$(CC) $(CFLAGS) -O0 $(SRCS) -o $@

$(BUILD)/t_O2: $(SRCS) | $(BUILD)
	$(CC) $(CFLAGS) -O2 $(SRCS) -o $@

$(BUILD)/t_san: $(SRCS) | $(BUILD)
	$(CC) $(CFLAGS) -O1 -fsanitize=address,undefined $(SRCS) -o $@

$(BUILD):
	mkdir -p $(BUILD)

clean:
	rm -rf $(BUILD) *.gcov *.gcda *.gcno