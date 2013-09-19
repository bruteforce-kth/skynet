all: main.cpp board.cpp
	g++ -std=c++11 main.cpp board.cpp -o main
clean: 
	rm -f main
	
