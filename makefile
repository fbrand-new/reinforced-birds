EXE = main.x
CXX = g++
CXXFLAGS = -I include -std=c++11

all: $(EXE)

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

$(EXE): main.o src/Observable.o src/State.o
	$(CXX) $^ -o $(EXE)

main.o: src/Observable.o src/State.o

src/Observable.o: include/Observable.h
src/State.o: include/State.h

clean:
	rm -rf src/*.o *.o $(EXE) */*~ *~ a.out*

.PHONY: clean all format