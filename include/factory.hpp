#ifndef NET_SIMULATION_FACTORY_HPP
#define NET_SIMULATION_FACTORY_HPP


#include <types.hpp>
#include <nodes.hpp>
#include <sstream>
#include <iostream>


template<typename Node>
class NodeCollection{
public:

    using container_t = typename std::list<Node>;
    using iterator = typename container_t::iterator;
    using const_iterator = typename container_t::const_iterator;


    const_iterator cbegin() const { return node_collection_.cbegin(); }
    const_iterator cend() const { return node_collection_.cend(); }
    const_iterator begin() const { return node_collection_.cbegin(); }
    const_iterator end() const { return node_collection_.cend(); }
    iterator begin() { return node_collection_.begin(); }
    iterator end() { return node_collection_.end(); }

    void add(Node&& node) { node_collection_.template emplace_back(std::move(node)); };

    const_iterator find_by_id(ElementID id) const { return std::find_if(begin(), end(),
                                                                        [id](const auto& elem) { return (elem.get_id() == id);});}

    iterator find_by_id(ElementID id) { return std::find_if(begin(), end(),
                                                            [id](auto& elem){ return (elem.get_id() == id);});}

    void remove_by_id(ElementID id) { if(find_by_id(id) != node_collection_.end()) node_collection_.erase(find_by_id(id)); }

private:
    container_t node_collection_;
};


enum class NodeColor { UNVISITED,
                        VISITED,
                        VERIFIED 
};


class Factory{
public:
    // Rampa
    void add_ramp(Ramp&& ramp) { ramp_collection_.add(std::move(ramp));}
    void remove_ramp(ElementID id);
    NodeCollection<Ramp>::iterator find_ramp_by_id(ElementID id) { return ramp_collection_.find_by_id(id);}
    NodeCollection<Ramp>::const_iterator find_ramp_by_id(ElementID id) const { return ramp_collection_.find_by_id(id);}
    NodeCollection<Ramp>::const_iterator ramp_cbegin() const { return ramp_collection_.cbegin();}
    NodeCollection<Ramp>::const_iterator ramp_cend() const { return ramp_collection_.cend();}

    // Pracownik
    void add_worker(Worker&& worker) { worker_collection_.add(std::move(worker));}
    void remove_worker(ElementID id);
    NodeCollection<Worker>::iterator find_worker_by_id(ElementID id) { return worker_collection_.find_by_id(id);}
    NodeCollection<Worker>::const_iterator find_worker_by_id(ElementID id) const { return worker_collection_.find_by_id(id);}
    NodeCollection<Worker>::const_iterator worker_cbegin() const { return worker_collection_.cbegin();}
    NodeCollection<Worker>::const_iterator worker_cend() const { return worker_collection_.cend();}

    // Magazyn
    void add_storehouse(Storehouse&& storehouse) { storehouse_collection_.add(std::move(storehouse));}
    void remove_storehouse(ElementID id);
    NodeCollection<Storehouse>::iterator find_storehouse_by_id(ElementID id) { return storehouse_collection_.find_by_id(id);}
    NodeCollection<Storehouse>::const_iterator find_storehouse_by_id(ElementID id) const { return storehouse_collection_.find_by_id(id);}
    NodeCollection<Storehouse>::const_iterator storehouse_cbegin() const { return storehouse_collection_.cbegin();}
    NodeCollection<Storehouse>::const_iterator storehouse_cend() const { return storehouse_collection_.cend();}

    bool is_consistent();

    void do_deliveries(Time t);

    void do_package_passing();

    void do_work(Time t);

private:
    NodeCollection<Worker> worker_collection_;
    NodeCollection<Ramp> ramp_collection_;
    NodeCollection<Storehouse> storehouse_collection_;

    template<typename Node>
    void remove_receiver(NodeCollection<Node>& collection , ElementID id );

};

template<typename Node>
void Factory::remove_receiver(NodeCollection<Node>& collection, ElementID id) {
    for(auto& node: collection){
        auto& map = node.receiver_preferences_.preferences_t_;
        auto ptr_1 = std::find_if(std::begin(map), std::end(map),[id](auto& pair){ return pair.first->get_id() == id;} );
        auto ptr_2 = ptr_1->first;
        node.receiver_preferences_.remove_receiver(ptr_2);
    }
}


bool has_reachable_storehouse(const PackageSender* sender, std::map<const PackageSender*, NodeColor>& node_colors);


Factory load_factory_structure(std::istream& is);


void save_factory_structure(Factory& factory, std::ostream& os);


enum ElementType{
    LOADING_RAMP,
    WORKER,
    STOREHOUSE,
    LINK
};


struct ParsedLineData{
    ElementType element_type_;
    std::map<std::string, std::string> parameters;
};


ParsedLineData parse_line(std::string line);


std::string en_to_string_queue(PackageQueueType type);
std::string en_to_string_receiver(ReceiverType type);


std::vector<IPackageReceiver*> find_receivers(const PackageSender& sender);
#endif //NET_SIMULATION_FACTORY_HPP
