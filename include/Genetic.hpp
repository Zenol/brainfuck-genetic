#ifndef GENETIC_H_
#define GENETIC_H_

#include "Vm.hpp"

#include <functional>
#include <set>

//! A generation is a set of virtual machine
typedef std::vector<Code> Generation;

//! Generate random adn (can be invalid)
std::string random_adn(int code_length);

//! Simplification of genetic code
std::string simplify(std::string code);

//! Repair broken adn.
//!
//! Its usefull for creating valid random adn, or after
//! a mutation.
std::string repair(std::string);

//! Create a random generation
Generation random_generation(int size, int code_length);

//! Merge genetic code by alternating instructions of each program.
std::string merge_from_start(const std::string &a, const std::string &b);
void cross_over(std::string &a, std::string &b);

std::string multi_cross_over(const std::string &a, const std::string &b, int block_size);

//! Replace some random genes of 'code' with probability 'p'
std::string mutate_replace(const std::string &code, float p);
//! Add some random genes to 'code' with probability 'p'
std::string mutate_insert(const std::string &code, float p);
//! Remove some random genes from 'code' with probability 'p'
std::string mutate_delete(const std::string &code, float p);

//! Sort a generation by using the fitness function
std::set<std::pair<int, std::string>>
    sort_generation(std::vector<VM> gen, std::function<int (const VM&)> f);

#endif
