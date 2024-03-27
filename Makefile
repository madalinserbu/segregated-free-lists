CC = gcc
CFLAGS = -Wall -Wextra -std=c99

# Sursele .c și .h
SRCS = allocator.c

# Fișierele obiect
OBJS = $(SRCS:.c=.o)

# Executabilul
TARGET = sfl

.PHONY: all clean run_sfl build

# Regula implicită
all: build

# Regula pentru build
build: $(TARGET)

# Regula pentru compilare
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Regula pentru rulează executabilul sfl
run_sfl: $(TARGET)
	./$(TARGET)

# Regula pentru șterge executabilul și fișierele obiect
clean:
	rm -f $(OBJS) $(TARGET)

# Rule pentru compilare fișierele .c în fișiere obiect
%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c -o $@ $<
