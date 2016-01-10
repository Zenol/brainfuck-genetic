#include "Vm.hpp"
#include "Genetic.hpp"
#include <iostream>

int main()
{
    std::string input, line;
    while(std::cin >> line)
        input += line;
    VM vm(input);
    vm.run();
    std::cout << vm.output << std::endl;
    return 0;
}
