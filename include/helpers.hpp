#ifndef NET_SIMULATION_HELPERS_HPP_
#define NET_SIMULATION_HELPERS_HPP_

#include <types.hpp>
#include <random>
#include <cstdlib>
#include <functional>

extern std::mt19937 rng;
extern std::random_device rd;
extern double default_probability_generator();
extern ProbabilityGenerator probability_generator;

#endif /* NET_SIMULATION_HELPERS_HPP_ */
