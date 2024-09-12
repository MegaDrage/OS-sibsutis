CC = gcc
CFLAGS = -Wall -Wextra -fPIC

LIB_NAME = liblist.so
TARGET = task1.out
TARGET2 = task2.out

SOURCES =  data.c list.c list_helpers.c list_navigation.c serialize_list.c
OBJECTS = $(SOURCES:.c=.o)

.PHONY: all clean run-task1 run-task2

all: $(LIB_NAME) $(TARGET) $(TARGET2)

$(LIB_NAME): $(OBJECTS)
	$(CC) -shared -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): main.c $(LIB_NAME)
	$(CC) -o $@ $< -L. -llist

$(TARGET2): task2.c $(LIB_NAME)
	$(CC) -o $@ $< -L -llist

clean:
	rm -rf *.o *.out *.so
run-task1: $(TARGET)
	LD_LIBRARY_PATH=. ./$(TARGET)
run-task2: $(TARGET)
	LD_LIBRARY_PATH=. ./$(TARGET2)
