#include "Vm.hpp"

#include <iostream>

int main()
{
    std::string pg, line;
    while (std::cin >> line)
        pg += line;
    VM vm(pg);
    vm.run("");
    std::cout << vm.output;
    return 0;
}
