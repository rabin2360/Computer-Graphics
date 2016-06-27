# Example 16
EXE=final

# Main target
all: $(EXE)

#  MinGW
ifeq "$(OS)" "Windows_NT"
CFLG=-O3 -Wall -DUSEGLEW
LIBS=-lglew32 -lglut32cu -lglu32 -lopengl32
CLEAN=del *.exe *.o *.a
else
#  OSX
ifeq "$(shell uname)" "Darwin"
CFLG=-O3 -Wall -Wno-deprecated-declarations
LIBS=-framework GLUT -framework OpenGL
#  Linux/Unix/Solaris
else
CFLG=-O3 -Wall -w -std=c++11
LIBS=-lglut -lGL -lGLU -lGLEW -lm
endif
#  OSX/Linux/Unix/Solaris
CLEAN=rm -f $(EXE) *.o *.a
endif

# Dependencies
final.o: final.cpp
utils.o: utils.cpp final.h
spehere.o: sphere.cpp final.h
particle.o: particle.cpp final.h
loadtexbmp.o: loadtexbmp.c CSCI5229.h
fatal.o: fatal.c CSCI5229.h
errorMsg.o: errorMsg.c CSCI5229.h
drawAxes.o: drawAxes.c CSCI5229.h
print.o: print.c CSCI5229.h
object.o: object.c CSCI5229.h
noise.o: noise.c CSCI5229.h

#  Create archive
CSCIx229.a:utils.o sphere.o particle.o loadtexbmp.o fatal.o errorMsg.o drawAxes.o print.o object.o noise.o
	ar -rcs $@ $^

# Compile rules
.c.o:
	gcc -c $(CFLG) $<
.cpp.o:
	g++ -c $(CFLG) $<

#  Link
final:final.o CSCIx229.a
	g++ -O3 -o $@ $^   $(LIBS)

#  Clean
clean:
	$(CLEAN)
