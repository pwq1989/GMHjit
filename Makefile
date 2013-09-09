WARN = -Wall -Wextra
DBG = -ggdb
FLAGS = -std=c++11 -fpermissive -g

all: main.o parser.o interpreter.o
	g++ $(WARN) $(DBG) $(FLAGS) -o gmhjit main.o parser.o

parser.o: parser.cpp parser.h type.h exceptions.h
	g++ $(WARN) $(DBG) $(FLAGS) -c parser.cpp

interpreter.o: interpreter.cpp interpreter.h type.h gmhjit.h exceptions.h
	g++ $(WARN) $(DBG) $(FLAGS) -c interpreter.cpp

gmhjit.h: gmhjit.dasc
	lua dynasm/dynasm.lua gmhjit.dasc > gmhjit.h
	
main.o: main.cpp parser.h type.h
	g++ $(WARN) $(DBG) $(FLAGS) -c main.cpp
clean:
	rm *.o gmhjit
