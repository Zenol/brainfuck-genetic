#ifndef VM_HPP_
#define VM_HPP_

#include <string>
#include <vector>

struct Ribbon
{
    Ribbon();

    void left();
    void right();
    char v;

private:
    std::vector<char> vleft;
    std::vector<char> vright;
};

// Assert that the input string is valid brainfuck code
class VM
{
public:
    VM(std::string code);

    bool run(std::string input, int max_cycles = 5000);

    std::string output;
    Ribbon ribbon;
    std::string code;
};

#endif
