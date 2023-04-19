#ifndef NET_SIMULATION_PACKAGE_HPP
#define NET_SIMULATION_PACKAGE_HPP


#include <algorithm>
#include <iostream>
#include <set>
#include <types.hpp>


class Package{
public:

    Package();
    Package(ElementID ElementID): ElementID_(ElementID) {}
    Package(Package&& package);
    Package& operator=( Package&& package);
    const ElementID& get_id() const { return ElementID_; };
    ~Package();

private:
    inline static std::set<ElementID> assigned_IDs;
    inline static std::set<ElementID> freed_IDs;
    ElementID ElementID_;
};


#endif //NET_SIMULATION_PACKAGE_HPP
// DONE