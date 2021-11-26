EXE = main.x
CXX = g++
CXXFLAGS = -I include -std=c++14 -Wall -g
#CXXFLAGS = -I include -std=c++14 -Wall -O3
OBJECTS = src/DirectedAgent.o src/UndirectedAgent.o src/Environment.o \
		src/DirectedObs.o src/UndirectedObs.o src/Action.o src/Angle.o \
		src/RandomWalk.o src/Observable.o src/State.o src/Bird.o src/V.o src/Boltzmann.o \
		src/Chase.o src/Signal.o src/Timer.o

all: $(EXE)

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)
$(EXE): main.o $(OBJECTS)
	$(CXX) $^ -o $(EXE)

main.o: $(OBJECTS)

src/DirectedAgent.o: include/DirectedAgent.h include/BaseAgent.h 
src/UndirectedAgent.o: include/UndirectedAgent.h include/BaseAgent.h include/Bird.h
src/DirectedObs.o: include/DirectedObs.h
src/UndirectedObs.o: include/UndirectedObs.h
src/Environment.o: include/Environment.h include/BaseAgent.h
src/RandomWalk.o: include/RandomWalk.h
src/Observable.o: include/Observable.h
src/State.o: include/State.h include/Bird.h
src/Bird.o: include/Bird.h
src/V.o: include/V.h
src/Boltzmann.o: include/Boltzmann.h
src/Chase.o: include/Chase.h
src/Signal.o: include/Signal.h
src/Timer.o: include/Timer.h
src/Action.o: include/Action.h
src/Angle.o: include/Angle.h


clean:
	rm -rf src/*.o *.o $(EXE) */*~ *~ a.out*

.PHONY: clean all format