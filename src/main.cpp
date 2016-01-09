#include "Vm.hpp"
#include "Genetic.hpp"

#include <iostream>

int main()
{
    auto gen = random_generation(500, 40);
    for (auto vm : gen)
    {
        vm.run("34");
        std::cout << vm.output << std::endl;
    }
    return 0;
}
