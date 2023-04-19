#ifndef NET_SIMULATION_TYPES_HPP
#define NET_SIMULATION_TYPES_HPP

#include <cstdlib>
#include <iostream>
#include <random>
#include <ctime>
#include <functional>
#include <list>


using ElementID = int;
using Time = int;
using TimeOffset = int;

using ProbabilityGenerator = std::function<double()>;

#endif //NET_SIMULATION_TYPES_HPP
