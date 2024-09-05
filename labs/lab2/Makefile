CFLAGS= -Wall -Werror -Wextra -g
SRCS=$(wildcard *.c)
OBJS=$(SRCS:%.c=%.o)
TARGET=main.out

.PHONY: all clean 

all: $(TARGET)
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

clean:
	rm -rf *.o *.out
