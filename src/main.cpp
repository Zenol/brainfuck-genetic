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


#define GRAPH

char rand_char();

unsigned addition_cost(VM vm)
{
    unsigned cost = 0;
    static unsigned char counter = 0;

    counter = rand_char();
    for (int i = 0; i < 30; i++)
    {
        char a = (counter + i) % 13;
        char b = (counter * 3 + i) % 24;
        char c = a + b;

        vm.reset();
        vm.run({(char)(a+i), (char)(b)});

        // We expect the output to have the same length
        cost += 3 * std::abs((int)vm.output.length() - 1);

        for (int i = 0; i < vm.output.length(); i++)
            cost += std::abs((int)vm.output[i] - c);
    }

    // We try to be sure we read exactly the input
    cost += std::abs(vm.input_consumed - 2) * 256;

    // We expect programs to be short
    cost = cost * 10 + vm.code.length() ;

    return cost;
}


unsigned text_cost(VM vm, std::string expected_string)
{
    unsigned cost = 0;

    vm.reset();
    vm.run("");

    // We expect the output to have the same length
    if (vm.output.length() < expected_string.length())
        cost += 256 * (expected_string.length() - vm.output.length());
    else
        cost += 100 * (vm.output.length() - expected_string.length());

    // We count the distance beetween each letter
    auto len = std::min(vm.output.length(), expected_string.length());
    for (int i = 0; i < len; i++)
        cost += std::abs(vm.output[i] - expected_string[i]);

    // We expect programs to be short
    cost = cost * 20 + vm.code.length();

    return cost;
}


int main()
{
    typedef std::set<std::pair<unsigned int, std::string>> ScoredGen;
    auto gen = random_generation(5000, 80);
    ScoredGen gens;

    // Our goal
    std::string goal_string = "Hello world!";

    for (int i = 0; i < 45000; i++)
    {
        gens = ScoredGen();
        for (int i = 0; i < gen.size(); i++)
        {
            VM& vm = gen[i];
            auto fitness = text_cost(vm, goal_string);
            gens.insert(std::make_pair(fitness, vm.code));
        }

        // Break when we found a winner :
        auto v = VM(gens.begin()->second);
        v.run("");
        if (v.output == goal_string)
            break;

        // Compute mean
        unsigned int mean = 0;
        for (auto x : gens)
            mean += x.first;
        mean /= gens.size();

        #ifndef GRAPH

        if (i % 100 == 0)
            std::cout << "Gen:" << i
                      << " Best: "
                      << gens.begin()->first
                      << " Worst:"
                      << gens.rbegin()->first
                      << " Mean:"
                      << mean
                      << " Count:"
                      << gen.size()
                      << "\n      Out:"
                      << v.output
                      << "\n      Code:"
                      << gens.begin()->second
                      << std::endl;

        #else

        if (i % 1 == 0)
            std::cout << gens.begin()->first
                      << " "
                      << gens.rbegin()->first
                      << " "
                      << mean
                      << " "
                      << gens.begin()->second
                      << std::endl;
        #endif

        // New generation
        std::vector<VM> newGen;

        // Keep the 30 best guys
        auto it = gens.begin();
        for (int i = 0; i < 30; i++, it++)
            newGen.push_back(VM(simplify(it->second)));

        //DEBUG
        // it = gens.begin();
        // for (int i = 0; i < 10; i++, it++)
        // {
        //     std::cout << it->first << " " << simplify(it->second) << std::endl;
        // }

        it = gens.begin();
        for (int i = 0; i < 30; i++, it++)
        {
            newGen.push_back(VM(simplify(repair(mutate_insert(it->second, 0.01f)))));
            newGen.push_back(VM(simplify(repair(mutate_delete(it->second, 0.01f)))));
            newGen.push_back(VM(simplify(repair(mutate_replace(it->second, 0.01f)))));

            newGen.push_back(VM(simplify(repair(mutate_insert(it->second, 0.1f)))));
            newGen.push_back(VM(simplify(repair(mutate_delete(it->second, 0.1f)))));
            newGen.push_back(VM(simplify(repair(mutate_replace(it->second, 0.1f)))));
        }

        // Cross breeding of the 101 first champions (non symetric)
//         auto ti = gens.begin();
//         for (int i = 0; i < 100; i++, ti++)
//         {
//             auto tj = ti;
//             tj++;
//             const std::string& a = ti->second;
//             const std::string& b = tj->second;
//             newGen.push_back(VM(simplify(merge_from_start(a, b))));
// //            newGen.push_back(VM(simplify(repair(a + b))));
//         }

        std::swap(gen, newGen);
    }

    #ifndef GRAPH

    for (auto kv : gens)
    {
        VM vm(kv.second);
        vm.run("");
        auto fitness = text_cost(vm, "hello world!");
        std::cout << vm.code
                  << std::endl
                  << vm.output
                  << std::endl
                  << fitness
                  << std::endl
                  << std::endl;
    }

    #endif

    return 0;
}
