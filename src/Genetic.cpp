#include "Genetic.hpp"

#include <algorithm>
#include <random>
#include <map>


// -----------------
// -- Work on ADN --

//! Law of probabilities
std::map<char, int> law = {
    std::make_pair('[', 100),
    std::make_pair(']', 100),
    std::make_pair('.', 100),
    std::make_pair(',', 0),
    std::make_pair('>', 100),
    std::make_pair('<', 100),
    std::make_pair('-', 100),
    std::make_pair('+', 100)
//    std::make_pair('?', 100)
};

std::default_random_engine rd;
std::uniform_int_distribution<int> uniform_dist(0, 10000);

char random_gene()
{
    double p = uniform_dist(rd) / 10000.f;

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
    return '#';
}

std::string random_chromosome(int code_length)
{
    std::string s;
    while(code_length--)
        s.push_back(random_gene());
    return s;
}

std::string repair(std::string code)
{
    std::string out;

    int length = code.length();
    for (int i = 0; i < length; i++)
    {
        switch(code[i])
        {
        case '[':
        case ']':
        case '+':
        case '-':
        case '<':
        case '>':
        case '.':
        case ',':
            // Check validity of opening bracket
            if (code[i] == ']' && goto_bracket_end(i, code) == -1)
                continue;
            // Check validity of closing bracket
            if (code[i] == '[' && goto_bracket_begin(i, code) == -1)
                continue;
            out.push_back(code[i]);
            break;
        default:
            break;
        }
    }

    return out;
}

std::string simplify(std::string code)
{
    std::string out;

    // If code is empty, do nothing
    if (code.length() == 0)
        return code;

    //! Helper macro used to check if we simplify two characters
    #define simp(a, b)                         \
    if (code[i] == b)                          \
        if (out.length() && out.back() == a)   \
        {                                      \
            out.pop_back();                    \
            continue;                          \
        }                                      \

    // Simplify with previous chars
    for (int i = 0; i < code.length(); i++)
    {
        simp('+', '-');
        simp('-', '+');
        simp('[', ']');
        simp('<', '>');
        simp('>', '<');

        out.push_back(code[i]);
    }

    return out;
}

// -------------------------
// -- Work on generations --

Generation random_generation(int size, int code_length)
{
    Generation gen;

    while (size--)
        //Add a newprogram
        gen.push_back(random_chromosome(code_length));

    return gen;
}

ScoredGeneration score_generation(const Generation &generation,
                                  std::function<unsigned int(Code)> fitness_fct)
{
    ScoredGeneration scored_generation;
    for (int i = 0; i < generation.size(); i++)
    {
        auto fitness = fitness_fct(generation[i]);
        scored_generation.push_back(std::make_pair(fitness, generation[i]));
    }
    return scored_generation;
}
#include <iostream>
Code select_fps(ScoredGeneration &generation)
{
    double sum = 0;
    for (auto x : generation)
        sum += 1.f / x.first;

    std::uniform_real_distribution<double> uniform_dist(0, sum);

    auto idx = uniform_dist(rd);
    for (const auto &x : generation)
    {
        idx -= 1.f / x.first;
        if (idx <= 0)
            return x.second;
    }
    return generation.begin()->second;
}

// ---------------
// -- Mutations --

Code mutate_replace(const Code &code, double p)
{
    std::string out;
    for (auto c : code)
    {
        double q = uniform_dist(rd) / 10000.f;
        if (q < p)
            out.push_back(random_gene());
        else
            out.push_back(c);
    }
    return out;
}


std::string mutate_insert(const std::string &code, double p)
{
    std::string out;
    for (auto c : code)
    {
        double q = uniform_dist(rd) / 10000.f;
        if (q < p)
            out.push_back(random_gene());
        out.push_back(c);
    }
    return out;
}

std::string mutate_delete(const std::string &code, double p)
{
    std::string out;
    for (auto c : code)
    {
        double q = uniform_dist(rd) / 10000.f;
        if (q < p)
            continue;
        out.push_back(c);
    }
    return out;
}

std::string mutate_swap(const std::string &code, double p)
{
    if (code.length() > 1)
        return code;

    std::string out = code;
    double q = uniform_dist(rd) / 10000.f;
    if (q < p)
    {
        std::uniform_int_distribution<unsigned int> d(0, code.length() - 2);
        auto pos = d(rd);

        std::swap(out[pos], out[pos + 1]);
    }
    return out;
}

// ------------------
// -- Reproduction --

std::string merge_from_start(const std::string &a, const std::string &b)
{
    std::string out;

    auto pa = a.begin();
    auto pb = b.begin();
    while(pa != a.end() && pb != b.end())
    {
        out.push_back(*pa);
        out.push_back(*pb);
        pa++;
        pb++;
    }
    std::copy(pa, a.end(), std::back_inserter(out));
    std::copy(pb, b.end(), std::back_inserter(out));

    return out;
}

void cross_over(std::string &a, std::string &b, double p)
{
    double q = uniform_dist(rd) / 10000.f;
    if (q > p)
        return;

    std::string oa, ob;

    std::uniform_int_distribution<int> uniform_dist(1, a.length());
    int cut_index = uniform_dist(rd);

    oa += a.substr(0, cut_index);
    if (cut_index < b.length())
        oa += b.substr(cut_index, std::string::npos);

    ob += b.substr(0, cut_index);
    if (cut_index < a.length())
        ob += a.substr(cut_index, std::string::npos);

    a = oa;
    b = ob;
}

std::string multi_cross_over(const std::string &a, const std::string &b, int block_size)
{

    std::string out;

    auto pa = a.begin();
    auto pb = b.begin();
    double p = 0;
    int counter = block_size;
    while (pa != a.end() && pb != b.end())
    {
        // Reset probability each block_size block
        if (counter == block_size)
        {
            p = uniform_dist(rd) / 10000.f;
            counter = 0;
        }

        // Select from who come the block
        if (p > 0.5)
            out.push_back(*pa);
        else
            out.push_back(*pb);
        pa++;
        pb++;
    }
    std::copy(pa, a.end(), std::back_inserter(out));
    std::copy(pb, b.end(), std::back_inserter(out));

    return out;
}
