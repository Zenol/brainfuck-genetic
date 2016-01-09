#include "Genetic.hpp"

#include <random>
#include <map>


//Law of probabilities

std::map<char, int> law = {
    std::make_pair('[', 10),
    std::make_pair(']', 10),
    std::make_pair('.', 5),
    std::make_pair(',', 5),
    std::make_pair('>', 30),
    std::make_pair('<', 30),
    std::make_pair('-', 50),
    std::make_pair('+', 50)
};

std::default_random_engine rd;
std::uniform_int_distribution<int> uniform_dist(0, 1000);

static
char rand_char()
{
    float p = uniform_dist(rd) / 1000.f;

    // Sum all the weights
    int sum = 0;
    for (auto kv : law)
        sum += kv.second;

    p *= sum;
    for (auto kv : law)
    {
        if (kv.second >= p)
            return kv.first;
        p -= kv.second;
    }
    return '+';
}

std::string random_adn(int code_length)
{
    std::string s;
    while(code_length--)
        s.push_back(rand_char());
    return s;
}
