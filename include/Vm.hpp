#ifndef VM_HPP_
#define VM_HPP_

#include <string>
#include <vector>

typedef std::string Code;

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

    //! Run the vm with 'input' as the given input.
    bool run(std::string input = "", int max_cycles = 1600);

    //! Reset the VM (ribbon and output)
    void reset();

    int input_consumed;
    std::string output;
    Ribbon ribbon;
    Code code;
};

#endif
