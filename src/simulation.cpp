#include <simulation.hpp>

void simulate(Factory& f, TimeOffset d, std::function<void(Factory&, Time)> rf){
    if (f.is_consistent()){
        for (Time tour = 1; tour <= d; tour++){
               f.do_deliveries(tour);
               f.do_package_passing();
               f.do_work(tour);
               rf(f, tour);
        }
    }else{
        throw std::logic_error("Brak spojnosci fabryki.");
    }
}
