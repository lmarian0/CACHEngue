CROSS_COMPILE ?= arm-linux-gnueabi-
CC = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -Wextra -O2 -march=armv6 -marm -pthread
LDFLAGS = -static -pthread
TARGET_BIN = signal_app

all: $(TARGET_BIN)

$(TARGET_BIN): signal_app.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@file $@

clean:
	rm -f $(TARGET_BIN) $(TARGET_BIN)_native

.PHONY: all clean
