PROGRAM := bridge
SRCS := $(wildcard *.c)
HDRS := $(wildcard *.h)
OBJS := $(SRCS:.c=.o)
CC := gcc
CFLAGS := -Wall -Wextra -Werror -Wpedantic -std=c11 -g -D_DEFAULT_SOURCE
STYLE := "{BasedOnStyle: llvm, AllowShortFunctionsOnASingleLine: None, KeepEmptyLinesAtTheStartOfBlocks: false}"


.PHONY: all
all : $(PROGRAM)

$(PROGRAM) : $(OBJS)
	$(CC) -o $@ $^

%.o : %.c
	$(CC) $(CFLAGS) -c $<

.PHONY: format
format: $(SRCS)
	clang-format -style=$(STYLE) -i $(SRCS) $(HDRS)

.PHONY: clean
clean:
	rm -f $(OBJS) $(PROGRAM)

.PHONY: run
run: $(PROGRAM)
	./$(PROGRAM)
