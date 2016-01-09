#include "Genetic.hpp"

#include <random>
#include <map>


//Law of probabilities

std::map<char, int> law = {
    std::make_pair('[', 10),
    std::make_pair(']', 10),
    std::make_pair('.', 8),
    std::make_pair(',', 7),
    std::make_pair('>', 70),
    std::make_pair('<', 70),
    std::make_pair('-', 100),
    std::make_pair('+', 270)
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

std::string repair(std::string code)
{
    std::string out;

    int lvl = 0;
    int length = code.length();
    for (int i = 0; i < length; i++)
    {
        if (code[i] == ']' && lvl == 0)
            continue;
        if (code[i] == '[')
            lvl++;
        if (code[i] == ']')
            lvl--;
        out.push_back(code[i]);
    }
    while(lvl-- > 0)
        out.push_back(']');
    return out;
}


std::string simplify(std::string code)
{
    std::string out;

    int length = code.length();
    for (int i = 0; i < length; i++)
    {
        if (code[i] == '+')
            if (i + 1 < length && code[i+1] == '-')
            {
                i += 2;
                continue;
            }
        if (code[i] == '[')
            if (i + 1 < length && code[i+1] == ']')
            {
                i += 2;
                continue;
            }
        if (code[i] == '-')
            if (i + 1 < length && code[i+1] == '+')
            {
                i += 2;
                continue;
            }
        out.push_back(code[i]);
    }
    return out;
}

Generation random_generation(int size, int code_length)
{
    Generation gen;

    while (size--)
    {
        //Add a newprogram
        auto code = random_adn(code_length);
        code = repair(code);
        code = simplify(code);
        gen.push_back(VM(code));
    }

    return gen;
}
