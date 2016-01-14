#ifndef VM_HPP_
#define VM_HPP_

#include <string>
#include <vector>

//! Type that represent a brainfuck code.
typedef std::string Code;

//! An 'infinite' bidirectional ribbon.
//!
//! It's a zipper.
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

class VM
{
public:
    VM(std::string code);

    //! Run the vm with 'input' as the given input.
    bool run(std::string input = "", int max_cycles = 15000);

    //! Reset the VM (ribbon and output).
    void reset();

    int input_consumed;
    bool killed;
    std::string output;
    Ribbon ribbon;
    Code code;
};

// -- Helpers functions :

//! Locate the index of the opening bracket for the closing bracket
//! located at location i.
//!
//! \return location of the opening bracket, -1 if failed.
int goto_bracket_begin(int i, const std::string& code);

//! Locate the index of the closing bracket for the opening bracket
//! located at location i.
//!
//! \return location of the closing bracket, -1 if failed.
int goto_bracket_end(int i, const std::string &code);

#endif
