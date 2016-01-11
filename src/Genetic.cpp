#include "Genetic.hpp"

#include <algorithm>
#include <random>
#include <map>

//Law of probabilities

std::map<char, int> law = {
    std::make_pair('[', 100),
    std::make_pair(']', 100),
    std::make_pair('.', 100),
    std::make_pair(',', 0),
    std::make_pair('>', 100),
    std::make_pair('<', 100),
    std::make_pair('-', 100),
    std::make_pair('+', 100),
    std::make_pair('#', 100)
};

std::default_random_engine rd;
std::uniform_int_distribution<int> uniform_dist(0, 10000);

char rand_char()
{
    float p = uniform_dist(rd) / 10000.f;

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
        // Forget about closing bracket with no use
        if (code[i] == ']' && lvl <= 0)
            continue;
        // Count opening bracket
        if (code[i] == '[')
            lvl++;
        // Count closing backet
        if (code[i] == ']')
            lvl--;
        out.push_back(code[i]);
    }
    // Add missing closing brackets
    while(lvl-- > 0)
        out.push_back(']');

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

Generation random_generation(int size, int code_length)
{
    Generation gen;

    while (size--)
        //Add a newprogram
        gen.push_back(random_adn(code_length));

    return gen;
}

std::set<std::pair<int, std::string>>
    sort_generation(std::vector<VM> gen, std::function<int (const VM&)> f)
{
    std::set<std::pair<int, std::string>> set;

    for (auto vm : gen)
        set.insert(std::make_pair(f(vm), vm.code));

    return set;
}

std::string mutate_replace(const std::string &code, float p)
{
    std::string out;
    for (auto c : code)
    {
        float q = uniform_dist(rd) / 10000.f;
        if (q < p)
            out.push_back(rand_char());
        else
            out.push_back(c);
    }
    return out;
}


std::string mutate_insert(const std::string &code, float p)
{
    std::string out;
    for (auto c : code)
    {
        float q = uniform_dist(rd) / 10000.f;
        if (q < p)
            out.push_back(rand_char());
        out.push_back(c);
    }
    return out;
}

std::string mutate_delete(const std::string &code, float p)
{
    std::string out;
    for (auto c : code)
    {
        float q = uniform_dist(rd) / 10000.f;
        if (q < p)
            continue;
        out.push_back(c);
    }
    return out;
}


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

void cross_over(std::string &a, std::string &b)
{

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
    float p = 0;
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
