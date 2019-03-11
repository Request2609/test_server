CC = g++
CXXFLAGS=-lpthread -g
target: main.cpp func.o socketfd.o
	$(CC) main.cpp func.o socketfd.o -o main $(CXXFLAGS)
func.o: func.cpp
	$(CC) -c func.cpp $(CXXFLAGS)
socketfd.o: socketfd.cpp
	$(CC) -c socketfd.cpp $(CXXFLAGS)
clean:
	rm *.o
