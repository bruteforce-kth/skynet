#include <iostream>
#include <cstdlib>
#include <vector>
#include <math.h>
#include <fstream>
#include "board.h"
#include "solver.h"

int main(int argc, char **argv)
{

    using std::cout;
    using std::endl;
    using std::ofstream;
    
    std::vector<std::vector<char> > chars;

    // Read the board
    for (std::string line; std::getline(std::cin, line);)
        chars.push_back(std::vector<char>(line.begin(), line.end()));

    // initalize the board
    board b1(chars);
    solver s;

    std::string solution = s.solve(b1);
    cout << solution << "\n";

    // ofstream myfile;
    // myfile.open ("Path.txt");
    // cout << "Writing solution to file 'Path.txt'.\n";
    // myfile << solution << "\n";
    // myfile.close();

    return 0;
}

