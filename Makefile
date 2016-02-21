PROGRAM := bridge
SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)
CC := gcc
CFLAGS := -Wall -Wextra -Werror -Wpedantic -std=c11 -D_DEFAULT_SOURCE

.PHONY: all
all : $(PROGRAM)

$(PROGRAM) : $(OBJS)
	$(CC) -o $@ $^

%.o : %.c
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean:
	rm -f $(OBJS) $(PROGRAM)

.PHONY: run
run: $(PROGRAM)
	./$(PROGRAM)
