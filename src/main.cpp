#include "Vm.hpp"
#include "Genetic.hpp"

#include <iostream>
#include <cmath>

// Goal :
// Generate a program that compute the sum of the two char
//
// Ex : 23 -> 5
//      00 -> 0
//      81 -> 9

int fitness_text(const VM &vm, const std::string &expected_output)
{
    int cost = 0;

    // We expect programs to be short
    cost += vm.code.length();

    // We expect the output to have the same length
    cost += 500 * std::abs((int)vm.output.length() - (int)expected_output.length());

    auto length = std::max(vm.output.length(), expected_output.length());
    for (int i = 0; i < length; i++)
        cost += std::abs(vm.output[i] - expected_output[i]);
    return cost;
}

int main()
{
    auto gen = random_generation(500, 30);
    for (auto &vm : gen)
    {
        vm.run("34");std::cout << vm.output.length() << " "; }
    auto gens = sort_generation(gen, [](const VM &vm){return fitness_text(vm, "7");});
    for (auto kv : gens)
        std::cout << kv.first << " " << kv.second << std::endl;
    return 0;
}
