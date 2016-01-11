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


#define GRAPH

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
        cost += 200 * (vm.output.length() - expected_string.length());

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
    ScoredGeneration scored_generation;
    auto generation = random_generation(1000, 80);

    // Our goal
    std::string goal_string = "Hello world!";

    // Evolution loop
    for (int i = 0; /* i < 4500 */ true; i++)
    {
        // Create a list of all the code with fitness
        scored_generation = score_generation(generation,
                                             [=](Code code) {
                                                 return text_cost(code, goal_string);
                                             });

        // Sort elements
        std::sort(scored_generation.begin(), scored_generation.end());

        // Break if we found a winner :
        if (i % 30 == 0)
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
            auto w = scored_generation.begin();
            auto vm = VM(w->second);
            vm.run();
            if (i % 100 == 0)
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

        if (i % 30 == 0)
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
        {
            std::string prev;
            auto it = scored_generation.begin();
            for (int i = 0; i < 10; i++)
            {
                while (prev == it->second)
                    it++;
                new_generation.push_back(it->second);
                prev = it->second;
                it++; // TODO : !!!! Look at scored_generation.end()
            }
        }


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

            std::string code1 = ti->second;
            std::string code2 = tj->second;

            cross_over(code1, code2);

            // code1 = mutate_insert(code1, 0.001f);
            // code1 = mutate_delete(code1, 0.001f);

            code1 = mutate_replace(code1, 0.1f);
            code2 = mutate_replace(code2, 0.1f);

            new_generation.push_back(code1);
            new_generation.push_back(code2);

            std::string code = multi_cross_over(ti->second, tj->second, 3);
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
