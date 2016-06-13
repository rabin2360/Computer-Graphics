#Makefile for c program
CC = gcc

#flags
CFLAGS =-O3 -Wall -w
GLUTFLGS = -lglut -lGL -lGLU -lm -Wno-write-strings

#target
TARGET = hw2

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) $(TARGET).c -o $(TARGET) $(GLUTFLGS)

#remove
clean:
	$(RM) $(TARGET) *.o *.a
