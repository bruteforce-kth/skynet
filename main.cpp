#include <iostream>
#include <cstdlib>
#include <vector>
#include <math.h>
#include "board.h"
#include "solver.h"

int main(int argc, char **argv)
{

    using std::cout;
    using std::endl;
    
    std::vector<std::vector<char> > chars;

    // Read the board
    for (std::string line; std::getline(std::cin, line);)
        chars.push_back(std::vector<char>(line.begin(), line.end()));

    // initalize the board
    board b1(chars);
    solver s;

    std::string solution = s.solve(b1);

    cout << solution << "\n";
    return 0;
}

