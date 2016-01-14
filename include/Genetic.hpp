#ifndef GENETIC_H_
#define GENETIC_H_

#include "Vm.hpp"

#include <functional>
#include <set>

//! A generation is a set of virtual machine.
typedef std::vector<Code> Generation;
typedef std::vector<std::pair<unsigned int, Code>> ScoredGeneration;

// -----------------
// -- Work on ADN --

//! Give a random gene.
char random_gene();

//! Generate random chromosome (select random genes) of length 'code_length'.
std::string random_chromosome(int code_length);

//! Simplification of genetic code (ex: >+< => +).
std::string simplify(std::string code);

//! Repair adn by removing broken genes.
std::string repair(std::string);

// -------------------------
// -- Work on generations --

//! Create a random generation
Generation random_generation(int size, int code_length);

//! Create a list of all the code with fitness
ScoredGeneration score_generation(const Generation &generation,
                                  std::function<unsigned int(Code)> fitness_fct);

//! Select a chromosome with FSP selection.
Code select_fps(ScoredGeneration &generation);

// ------------------
// -- Reproduction --

//! Merge genetic code by alternating instructions of each program.
std::string merge_from_start(const std::string &a, const std::string &b);

//! Select a random location in a. then split a and b at the same index,
//  and swap the tail of those two. This operation occure in place.
// This happend with probability 'p'.
void cross_over(std::string &a, std::string &b, double p);

//! Interleave a and b genes by chuck of size 'block_size'.
std::string multi_cross_over(const std::string &a, const std::string &b, int block_size);

// ---------------
// -- Mutations --

//! For each gene, replace it with a random one with probability 'p'
std::string mutate_replace(const std::string &code, double p);
//! For each gene, insert a random gene ahead with probability 'p'
std::string mutate_insert(const std::string &code, double p);
//! For each gene, remove it with probability 'p'
std::string mutate_delete(const std::string &code, double p);
//! With probability p, swap two adjacent genes
std::string mutate_swap(const std::string &code, double p);

#endif
