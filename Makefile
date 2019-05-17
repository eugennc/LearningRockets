HOMEWORKNAME=learningRockets
TEAMNAME=croitoru eugen
PREPEND=[gpc]
CC=g++
CFLAGS=-Wall -c -ggdb -emacs -O0
CPPFLAGS=$(CFLAGS)
GENERALFLAGS=-std=c++98
LDFLAGS=-lm -lGL -lGLU -lGLEW -lSDL2
CPPSOURCES=$(wildcard ./src/*.cpp)
CPPOBJECTS=$(CPPSOURCES:.cpp=.o)
CSOURCES=$(wildcard ./src/*.c)
SOURCES=$(CSOURCES) $(CPPSOURCES)
HEADERS=$(CSOURCES:.c=.h) $(CPPSOURCES:.cpp=.h)
OBJECTS=$(COBJECTS) $(CPPOBJECTS)
CEXES=$(CSOURCES:.c=)
CPPEXES=$(CPPSOURCES:.cpp=)
EXECUTABLE=$(HOMEWORKNAME)
ZIPNAME="$(PREPEND) $(TEAMNAME) $(HOMEWORKNAME).zip"
CLEANUP=$(EXECUTABLE) $(ZIPNAME) $(wildcard ./*~) $(OBJECTS) $(CSOURCES:.c=.c~) $(CPPSOURCES:.cpp=.cpp~) $(HEADERS:.h=.h~)



all: $(OBJECTS) $(EXECUTABLE)

debug:
	make CFLAGS="-Wall -c -ggdb -emacs -O0"

release:
	make CFLAGS="-Wall -c -DNDEBUG -O3"

.cpp.o:
	$(CC) $(GENERALFLAGS) $(CPPFLAGS) $< -o $@

.c.o:
	$(CC) $(GENERALFLAGS) $(CFLAGS) $< -o $@

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(GENERALFLAGS) $(LDFLAGS)

clean:
	rm -rf $(CLEANUP) 2>&1 || echo ""

zip: clean
	zip -r $(ZIPNAME) . -x .\* \*~ \*\#

run: $(EXECUTABLE)
	./$(EXECUTABLE)

perf: $(EXECUTABLE)
	rm callgrind.out.*
	valgrind --tool=callgrind ./$(EXECUTABLE)
	gprof2dot -f callgrind callgrind.out.* | dot -Tsvg -o perf.svg
	inkscape perf.svg
