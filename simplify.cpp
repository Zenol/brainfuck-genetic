#include "Vm.hpp"
#include "Genetic.hpp"
#include <iostream>

int main()
{
    std::string input, line;
    while(std::cin >> line)
        input += line;

    std::cout << simplify(input) << std::endl;

    return 0;
}
