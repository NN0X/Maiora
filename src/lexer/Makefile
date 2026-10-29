CC = clang
TARGET = main
CFLAGS = -O3
WARNINGS = -Wall -Wextra -Wpedantic -Werror \
           -Wconversion -Wshadow -Wundef \
           -Wformat -Wformat-security \
           -Wstrict-prototypes -Wmissing-prototypes \
           -Werror=implicit-function-declaration \
           -Werror=return-type \
           -Wno-unused-parameter \
           -Wunreachable-code \
           -Wcast-align

FLAGS = $(CFLAGS) $(WARNINGS) -std=gnu17 -g

SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

%.o: %.c
	$(CC) $(FLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
