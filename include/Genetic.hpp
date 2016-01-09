#ifndef GENETIC_H_
#define GENETIC_H_

#include "Vm.hpp"

//! A generation is a set of virtual machine
typedef std::vector<VM> Generation;

//! Generate random adn (can be invalid)
std::string random_adn(int code_length);

//! Simplification of genetic code
std::string simplify(std::string code);

//! Repair broken adn.
//!
//! Its usefull for creating valid random adn, or after
//! a mutation.
std::string repair(std::string);

Generation random_generation(int size, int code_length);

#endif
