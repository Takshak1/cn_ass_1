CXX = g++
CXXFLAGS = -Wall -std=c++17

all: server client

server: server.cpp
	$(CXX) $(CXXFLAGS) -o server.out server.cpp

client: client.cpp
	$(CXX) $(CXXFLAGS) -o client.out client.cpp

clean:
	rm -f *.out result_table.txt
