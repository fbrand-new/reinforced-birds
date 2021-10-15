EXE = main.x
CXX = g++
CXXFLAGS = -I include -std=c++14 -Wall -g

all: $(EXE)

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

$(EXE): main.o src/Observable.o src/State.o src/Agent.o src/RandomWalk.o src/Environment.o src/Policy.o src/Bird.o src/V.o
	$(CXX) $^ -o $(EXE)

main.o: src/Observable.o src/State.o src/Agent.o src/RandomWalk.o src/Policy.o src/Environment.o src/Bird.o src/V.o

src/Agent.o: include/Agent.h include/State.h include/Observable.h include/Action.h include/Policy.h
src/Policy.o: include/Policy.h include/Action.h include/Observable.h
src/Environment.o: include/Environment.h
src/RandomWalk.o: include/Policy.h include/RandomWalk.h
src/Observable.o: include/Observable.h
src/State.o: include/State.h include/Bird.h
src/Bird.o: include/Bird.h
src/V.o: include/V.h

clean:
	rm -rf src/*.o *.o $(EXE) */*~ *~ a.out*

.PHONY: clean all format