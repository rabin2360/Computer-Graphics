# Example 16
EXE=ex1

# Main target
all: $(EXE)

#  MinGW
ifeq "$(OS)" "Windows_NT"
CFLG=-O3 -Wall
LIBS=-lglut32cu -lglu32 -lopengl32
CLEAN=del *.exe *.o *.a
else
#  OSX
ifeq "$(shell uname)" "Darwin"
CFLG=-O3 -Wall -Wno-deprecated-declarations
LIBS=-framework GLUT -framework OpenGL
#  Linux/Unix/Solaris
else
CFLG=-O3 -Wall -w -std=c++11
LIBS=-lglut -lGLU -lGL -lm
endif
#  OSX/Linux/Unix/Solaris
CLEAN=rm -f $(EXE) *.o *.a
endif

# Dependencies
ex1.o: ex1.cpp
utils.o: utils.cpp ex1.h
spehere.o: sphere.cpp ex1.h
particle.o: particle.cpp ex1.h
loadtexbmp.o: loadtexbmp.c CSCI5229.h
fatal.o: fatal.c CSCI5229.h
errorMsg.o: errorMsg.c CSCI5229.h
drawAxes.o: drawAxes.c CSCI5229.h
print.o: print.c CSCI5229.h

#  Create archive
CSCIx229.a:utils.o sphere.o particle.o loadtexbmp.o fatal.o errorMsg.o drawAxes.o print.o
	ar -rcs $@ $^

# Compile rules
.c.o:
	gcc -c $(CFLG) $<
.cpp.o:
	g++ -c $(CFLG) $<

#  Link
ex1:ex1.o CSCIx229.a
	g++ -O3 -o $@ $^   $(LIBS)

#  Clean
clean:
	$(CLEAN)
