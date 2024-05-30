CC=gcc
CFLAGS=-g -ggdb -std=c11 -pedantic -W -Wall -Wextra
CINCLUDE=-I$(SRC)
SRC=src
MODE=debug
BUILD=build
BUILD_OUT=$(BUILD)/$(MODE)
DIST=bin
TARGET=lab4
TARGET_PRODUCER=producer
TARGET_CONSUMER=consumer

ifeq ($(MODE), release)
	CFLAGS=-std=c11 -pedantic -W -Wall -Wextra -Werror
endif

rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

TARGET_SRCS=$(filter-out $(SRC)/$(TARGET_CONSUMER)/%, $(filter-out $(SRC)/$(TARGET_PRODUCER)/%, $(call rwildcard,$(SRC)/,*.c)))
TARGET_OBJS=$(patsubst $(SRC)/%.c, $(BUILD_OUT)/%.o, $(TARGET_SRCS))

PRODUCER_SRCS=$(wildcard $(SRC)/ring.c) $(call rwildcard,$(SRC)/$(TARGET_PRODUCER),*.c)
PRODUCER_OBJS=$(patsubst $(SRC)/%.c, $(BUILD_OUT)/%.o, $(PRODUCER_SRCS))

CONSUMER_SRCS=$(wildcard $(SRC)/ring.c) $(call rwildcard,$(SRC)/$(TARGET_CONSUMER),*.c)
CONSUMER_OBJS=$(patsubst $(SRC)/%.c, $(BUILD_OUT)/%.o, $(CONSUMER_SRCS))

all : $(TARGET) $(TARGET_PRODUCER) $(TARGET_CONSUMER)

$(BUILD_OUT)/%.o: $(SRC)/%.c
	@mkdir -p $(@D)
	@mkdir -p $(DIST)
	$(CC) $(CFLAGS) -o $@ -c $<

$(TARGET): $(TARGET_OBJS)
	$(CC) $(CFLAGS) $(CINCLUDE) -o $(DIST)/$(TARGET) $^

$(TARGET_PRODUCER): $(PRODUCER_OBJS)
	$(CC) $(CFLAGS) $(CINCLUDE) -o $(DIST)/$(TARGET_PRODUCER) $^

$(TARGET_CONSUMER): $(CONSUMER_OBJS)
	$(CC) $(CFLAGS) $(CINCLUDE) -o $(DIST)/$(TARGET_CONSUMER) $^

clean:
	rm -rf $(BUILD) $(DIST)