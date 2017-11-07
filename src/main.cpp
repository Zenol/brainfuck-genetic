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


//#define GRAPH

unsigned text_cost(std::string code, std::string expected_string)
{
    unsigned cost = 1;

    VM vm(code);
    vm.reset();
    vm.run("");

    // We expect the output to have bigger length
    if (vm.output.length() < expected_string.length())
        cost += 256 * (expected_string.length() - vm.output.length());

    // We count the distance beetween each letter
    auto len = std::min(vm.output.length(), expected_string.length());
    for (int i = 0; i < len; i++)
      cost += std::abs(vm.output[i] - expected_string[i]);

    return cost;
}


int main()
{
    ScoredGeneration scored_generation;
    auto generation = random_generation(5000, 200);

    // Our goal
    std::string goal_string = "Hello!";

    // Evolution loop
    for (int i = 0; /* i < 4500 */ true; i++)
    {
        // Create a list of all the code with fitness
        scored_generation = score_generation(generation,
                                             [=](Code code) {
                                                 return text_cost(code, goal_string);
                                             });

        Generation new_generation;

        for (int i = 0; i < 1000; i++)
        {
            std::string code1 = select_fps(scored_generation);
            std::string code2 = select_fps(scored_generation);

	    // Always crossover
            cross_over(code1, code2, 1.f);

            code1 = mutate_replace(code1, 5.0f/10000.f);
            code2 = mutate_replace(code2, 5.0f/10000.f);
            code1 = mutate_swap(code1, 5.0f/10000.f);
            code2 = mutate_swap(code2, 5.0f/10000.f);

            new_generation.push_back(code1);
            new_generation.push_back(code2);
        }

        // Sort elements
        std::sort(scored_generation.begin(), scored_generation.end());

        // Keep the k best guys
        // {
        //     std::string prev;
        //     auto it = scored_generation.begin();
        //     for (int i = 0; i < 10; i++)
        //     {
        //         while (prev == it->second)
        //             it++;
        //         new_generation.push_back(it->second);
        //         prev = it->second;
        //         it++; // TODO : !!!! Look at scored_generation.end()
        //     }
        // }


        // Break if we found a winner :
//        if (i % 30 == 0)
        {
            auto vm = VM(scored_generation.begin()->second);
            vm.run("");
            if (vm.output.substr(0, goal_string.length()) == goal_string)
                break;
        }

        // Compute mean
        unsigned int mean = 0;
        for (auto x : scored_generation)
            mean += x.first;
        mean /= scored_generation.size();

        #ifndef GRAPH
        {
            auto w = scored_generation.begin();
            auto vm = VM(w->second);
            vm.run();
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

//        if (i % 30 == 0)
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
        std::swap(generation, new_generation);
    }

    #ifndef GRAPH

    for (auto kv : scored_generation)
    {
        auto fitness = text_cost(kv.second, goal_string);
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
