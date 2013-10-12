all: main.cpp board.cpp solver.cpp
	g++ -std=c++11 main.cpp board.cpp solver.cpp -o main
clean: 
	rm -f main
	
