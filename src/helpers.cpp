#include <helpers.hpp>
#include <random>
#include <cstdlib>
#include <functional>

std::random_device rd;
std::mt19937 rng(rd());

double default_probability_generator() {
    return std::generate_canonical<double, 10>(rng);
}

std::function<double()> probability_generator = default_probability_generator;
