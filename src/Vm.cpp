#include "Vm.hpp"

#include <iostream>

// -- Ribbon implementation

Ribbon::Ribbon()
    :v(0)
{}


void Ribbon::left()
{
    if (vleft.empty())
        vleft.push_back(0);
    vright.push_back(v);
    v = vleft.back();
    vleft.pop_back();
}

void Ribbon::right()
{
    if (vright.empty())
        vright.push_back(0);
    vleft.push_back(v);
    v = vright.back();
    vright.pop_back();
}

// -- Vm implementation

VM::VM(std::string icode)
    :code(icode), input_consumed(0)
{}

void VM::reset()
{
    output = "";
    ribbon = Ribbon();
    input_consumed = 0;
}

// -- Helpers functions
int goto_bracket_end(int i, const std::string &code)
{
    int lvl = 0;
    i++;
    while (true)
    {
        if (i >= code.length())
            return -1;
        if (code[i] == '[')
            lvl++;
        if (code[i] == ']')
            lvl--;
        if (lvl == -1)
            return i;
        i++;
    }
}

int goto_bracket_begin(int i, const std::string &code)
{
    int lvl = 0;
    i--;
    while (true)
    {
        if (i <= 0)
            return -1;
        if (code[i] == ']')
            lvl++;
        if (code[i] == '[')
            lvl--;
        if (lvl == -1)
            return i;
        i--;
    }
}

// -- Run brainfuck code

bool VM::run(std::string input, int max_cycles)
{
    int input_idx = 0;
    for (int i = 0; i < code.length() && max_cycles; i++, max_cycles--)
    {
        switch(code[i])
        {
        case '+':
            ribbon.v++;
            break;
        case '-':
            ribbon.v--;
            break;
        case '>':
            ribbon.right();
            break;
        case '<':
            ribbon.left();
            break;
        case '.':
            output.push_back(ribbon.v);
            break;
        case ',':
            input_consumed++;
            if (i < input.length())
                ribbon.v = input[i];
            else
                ribbon.v = '\04';
            break;
        case '[':
            if (ribbon.v == 0)
            {
                int j = goto_bracket_end(i, code);
                if (j > -1)
                    i = j;
                continue;
            }
            break;
        case ']':
            if (ribbon.v != 0)
            {
                int j = goto_bracket_begin(i, code);
                if (j > -1)
                    i = j;
                continue;
            }
            break;
        default:
            //Ignore unknow characters
            continue;
        }
        if (i < 0)
            return false;
    }
    return true;
}
