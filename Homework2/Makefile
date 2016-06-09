CC = gcc

#flags
CFLAGS =-O3 -Wall
GLUTFLGS = -lglut -lGL -lGLU -lm

#target
TARGET = hw2

all: $(TARGET)

#compile
$(TARGET): $(TARGET).cpp
	$(CC) $(CFLAGS) $(TARGET).cpp -o $(TARGET) $(GLUTFLGS)

#remove
clean:
	$(RM) $(TARGET) *.o *.a
