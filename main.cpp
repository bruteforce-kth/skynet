#include <iostream>
#include <cstdlib>
#include <vector>
#include <math.h>
#include "board.h"

using std::cout;
using std::endl;

int main(int argc, char **argv)
{
    
    
    std::vector<std::vector<char> > chars;

    // Read the board
    for (std::string line; std::getline(std::cin, line);)
        chars.push_back(std::vector<char>(line.begin(), line.end()));

    // initalize the board
    board b1(chars);
    
    std::string solution = b1.solve();
    cout << solution << "\n";
    return 0;
}

