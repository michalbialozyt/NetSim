#include <package.hpp>

Package::Package(Package&& package) {

    ElementID_ = std::move(package.ElementID_);
}


Package & Package::operator=(Package &&package) {

    ElementID_ = std::move(package.ElementID_);
    return *this;
}


Package::Package() {

    if (freed_IDs.size() != 0){
        auto it = freed_IDs.begin();

        ElementID_ = *it;
        freed_IDs.erase(ElementID_);
        assigned_IDs.insert(ElementID_);

    }else{
        if (assigned_IDs.size() == 0){
            ElementID_ = 1;
            assigned_IDs.insert(1);
        }else{
            ElementID_ = *assigned_IDs.end();
            assigned_IDs.insert(ElementID_);
        }
    }
}

Package::~Package() {

    assigned_IDs.erase(ElementID_);
    freed_IDs.insert(ElementID_);
}

//DONE