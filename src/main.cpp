#include "Vm.hpp"
#include "Genetic.hpp"

#include <iostream>
#include <cmath>
#include <random>
#include <algorithm>

// Goal :
// Generate a program that compute the sum of the two char
//
// Ex : 23 -> 5
//      00 -> 0
//      81 -> 9


// #define GRAPH

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


unsigned text_cost(std::string code, std::string expected_string)
{
    unsigned cost = 1;

    VM vm(code);
    vm.reset();
    vm.run("");

    // We expect the output to have the same length
    if (vm.output.length() < expected_string.length())
        cost += 256 * (expected_string.length() - vm.output.length());
    else
        cost += 10 * (vm.output.length() - expected_string.length());

    // We count the distance beetween each letter
    auto len = std::min(vm.output.length(), expected_string.length());
    for (int i = 0; i < len; i++)
        cost += std::abs(vm.output[i] - expected_string[i]) + 10 * (vm.output[i] != expected_string[i]);

    // We expect programs to be short
    // TODO : Write a function that only count the effective
    //        code length.
    cost = cost * 20 + vm.code.length();

    return cost;
}


int main()
{
    typedef std::vector<std::pair<unsigned int, std::string>> ScoredGen;

    auto generation = random_generation(1000, 40);
    ScoredGen scored_generation;

    // Our goal
    std::string goal_string = "Hello!";

    // Evolution loop
    for (int i = 0; /* i < 4500 */ true; i++)
    {
        // Create a list of all the code with fitness
        scored_generation = ScoredGen();
        for (int i = 0; i < generation.size(); i++)
        {
            auto fitness = text_cost(generation[i], goal_string);
            scored_generation.push_back(
                std::make_pair(fitness, generation[i])
                );
        }

        // Sort elements
        std::sort(scored_generation.begin(), scored_generation.end());

        // Break when we found a winner :
        {
            auto vm = VM(scored_generation.begin()->second);
            vm.run("");
            if (vm.output == goal_string)
                break;
        }

        // Compute mean
        unsigned int mean = 0;
        for (auto x : scored_generation)
            mean += x.first;
        mean /= scored_generation.size();

        #ifndef GRAPH
        {
            auto vm = VM(scored_generation.begin()->second);
            vm.run();
            auto w = scored_generation.begin();
            if (i % 10 == 0)
                std::cout << "Gen:" << i
                          << " Best: "
                          << scored_generation.begin()->first
                          << " Worst:"
                          << scored_generation.rbegin()->first
                          << " Mean:"
                          << mean
                          << " Count:"
                          << generation.size()
                          << "\n      Out:"
                          << vm.output
                          << "\n      Code:"
                          << w++->second
                          << "\n      Code:"
                          << w++->second
                          << "\n      Code:"
                          << w++->second
                          << std::endl;
        }
        #else

        if (i % 1 == 0)
            std::cout << scored_generation.begin()->first
                      << " "
                      << scored_generation.rbegin()->first
                      << " "
                      << mean
                      << " "
                      << scored_generation.begin()->second
                      << std::endl;
        #endif

        // New generation
        Generation new_generation;

        // Keep the k best guys
        // for (int i = 0; i < 1; i++)
        //     new_generation.push_back(scored_generation[i].second);


        // Only mutate the first of the list
        // auto it = scored_generation.begin();
        // for (int i = 0; i < 30; i++, it++)
        // {
        //     std::string code = it->second;

        //     // code = mutate_insert(code, 0.01f);
        //     // code = mutate_delete(code, 0.01f);
        //     code = mutate_replace(code, 0.01f);
        //     new_generation.push_back(code);

        //     // code = mutate_insert(code, 0.1f);
        //     // code = mutate_delete(code, 0.1f);
        //     code = mutate_replace(code, 0.1f);
        //     new_generation.push_back(code);

        //     code = mutate_replace(code, 0.4f);
        //     new_generation.push_back(code);
        // }

        // Cross breeding of the 100 first champions (non symetric)

        std::default_random_engine rd;
        std::uniform_int_distribution<int> uniform_dist(0, 200);
        auto ti = scored_generation.begin();
        for (int i = 0; i < 200; i++, ti++)
        {
            auto tj = scored_generation.begin();
            int idx = uniform_dist(rd);
            while(idx--)
                tj++;

            const std::string& a = ti->second;
            const std::string& b = tj->second;

            std::string code;
            code = cross_over(a, b, 2);

            // code = mutate_insert(code, 0.001f);
            // code = mutate_delete(code, 0.001f);
            code = mutate_replace(code, 0.1f);
            new_generation.push_back(code);

            code = cross_over(a, b, 7);
            // code = mutate_insert(code, 0.1f);
            // code = mutate_delete(code, 0.1f);
            code = mutate_replace(code, 0.1f);
            new_generation.push_back(code);
        }

        std::swap(generation, new_generation);
    }

    #ifndef GRAPH

    for (auto kv : scored_generation)
    {
        auto fitness = text_cost(kv.second, "hello world!");
        VM vm(kv.second);
        vm.run("");
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
