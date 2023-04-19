#include <factory.hpp>

void Factory::do_deliveries(Time t) {
    for (auto& ramp: ramp_collection_){
        ramp.deliver_goods(t);
    }
}


void Factory::do_package_passing() {
    for (auto& elem: ramp_collection_){
        elem.send_package();
    }
    for (auto& elem: worker_collection_){
        elem.send_package();
    }
}


void Factory::do_work(Time t) {
    for (auto& worker: worker_collection_){
        worker.do_work(t);
    }
}


bool Factory::is_consistent() {

    std::map<const PackageSender*, NodeColor> node_colors;

    for (auto& ramp : ramp_collection_) {
        node_colors.insert(std::pair<const PackageSender*, NodeColor>(&ramp, NodeColor::UNVISITED));
    }

    for (auto& worker : worker_collection_) {
        node_colors.insert(std::pair<const PackageSender*, NodeColor>(&worker, NodeColor::UNVISITED));
    }

    for (auto& ramp : ramp_collection_) {
        try {
            has_reachable_storehouse(&ramp, node_colors);
        }
        catch (std::logic_error const&) {
            return false;
        }
    }

    return true;
}


void Factory::remove_ramp(ElementID id) {
    remove_receiver(worker_collection_, id);
    remove_receiver(ramp_collection_, id);
    ramp_collection_.remove_by_id(id);
}


void Factory::remove_storehouse(ElementID id) {
    remove_receiver(worker_collection_, id);
    remove_receiver(ramp_collection_, id);
    storehouse_collection_.remove_by_id(id);
}


void Factory::remove_worker(ElementID id) {
    remove_receiver(worker_collection_, id);
    remove_receiver(ramp_collection_, id);
    worker_collection_.remove_by_id(id);
}


bool has_reachable_storehouse(const PackageSender* sender, std::map<const PackageSender*, NodeColor>& node_colors){
    if (node_colors[sender] == NodeColor::VERIFIED){
        return true;
    }
    node_colors[sender] = NodeColor::VISITED;

    if(sender->receiver_preferences_.preferences_t_.empty()){
        throw std::logic_error("Nadawca nie posiada żadnego połączenia wyjściowego.");
    }

    bool has_another_receiver = false;

    for(auto& pair : sender->receiver_preferences_.preferences_t_){
        IPackageReceiver* pair_ptr = pair.first;

        if(pair_ptr->get_receiver_type() == ReceiverType::Storehouse){
            has_another_receiver = true;
        }else{
            const auto worker_ptr = dynamic_cast<Worker*>(pair_ptr);
            const PackageSender* sendrecv_ptr = dynamic_cast<PackageSender*>(worker_ptr);
            if(sendrecv_ptr == sender){
                continue;
            }
            has_another_receiver = true;
            if(node_colors[sendrecv_ptr] == NodeColor::UNVISITED){
                has_reachable_storehouse(sendrecv_ptr, node_colors);
            }

        }
    }
    node_colors[sender] = NodeColor::VERIFIED;

    if(has_another_receiver){
        return true;
    }else{
        throw std::logic_error("Nadawca nie posiada żadnego połączenia wyjściowego.");
    }
}


Factory load_factory_structure(std::istream& is){

    std::string line;
    Factory factory;
    ParsedLineData parsed_line;

    while(std::getline(is, line)){
        if(line.rfind(";", 0) == 0 || line.empty()){
            continue;
        }
        parsed_line = parse_line(line);
        switch (parsed_line.element_type_) {
            case ElementType::LOADING_RAMP:
                factory.add_ramp(Ramp(std::stoi(parsed_line.parameters.find("id")->second), std::stoi(parsed_line.parameters.find("delivery-interval")->second)));
                break;
            case ElementType::WORKER:
                if(parsed_line.parameters.find("queue-type")->second == "FIFO"){
                    factory.add_worker(Worker(std::stoi(parsed_line.parameters.find("id")->second), std::stoi(parsed_line.parameters.find("processing-time")->second), std::make_unique<PackageQueue>(PackageQueue(PackageQueueType::FIFO))));
                } else if(parsed_line.parameters.find("queue-type")->second == "LIFO"){
                    factory.add_worker(Worker(std::stoi(parsed_line.parameters.find("id")->second), std::stoi(parsed_line.parameters.find("processing-time")->second), std::make_unique<PackageQueue>(PackageQueue(PackageQueueType::LIFO))));
                }
                break;
            case ElementType::STOREHOUSE:
                factory.add_storehouse(Storehouse(std::stoi(parsed_line.parameters.find("id")->second)));
                break;
            case ElementType::LINK:
                if(parsed_line.parameters.find("src=ramp") != parsed_line.parameters.end() && parsed_line.parameters.find("dest=worker") != parsed_line.parameters.end()) {
                    auto& ramp = *(factory.find_ramp_by_id(std::stoi(parsed_line.parameters.find("src=ramp")->second)));
                    Worker& worker_receiver = *(factory.find_worker_by_id(std::stoi(parsed_line.parameters.find("dest=worker")->second)));
                    ramp.receiver_preferences_.add_receiver(&worker_receiver);

                } else if(parsed_line.parameters.find("src=worker") != parsed_line.parameters.end() && parsed_line.parameters.find("dest=worker") != parsed_line.parameters.end()) {
                    auto worker_source = factory.find_worker_by_id(std::stoi(parsed_line.parameters.find("src=worker")->second));
                    Worker& worker_receiver = *(factory.find_worker_by_id(std::stoi(parsed_line.parameters.find("dest=worker")->second)));
                    worker_source->receiver_preferences_.add_receiver(&worker_receiver);

                } else if(parsed_line.parameters.find("src=worker") != parsed_line.parameters.end() && parsed_line.parameters.find("dest=store") != parsed_line.parameters.end()) {
                    auto worker = factory.find_worker_by_id(std::stoi(parsed_line.parameters.find("src=worker")->second));
                    Storehouse& storehouse_receiver = *(factory.find_storehouse_by_id(std::stoi(parsed_line.parameters.find("dest=store")->second)));
                    worker->receiver_preferences_.add_receiver(&storehouse_receiver);

                } else if(parsed_line.parameters.find("src=ramp") != parsed_line.parameters.end() && parsed_line.parameters.find("dest=store") != parsed_line.parameters.end()) {
                    Ramp& ramp = *(factory.find_ramp_by_id(std::stoi(parsed_line.parameters.find("src=ramp")->second)));
                    Storehouse& storehouse_receiver = *(factory.find_storehouse_by_id(std::stoi(parsed_line.parameters.find("dest=store")->second)));
                    ramp.receiver_preferences_.add_receiver(&storehouse_receiver);

                }
                break;
        }
    }
    return factory;
}


void save_factory_structure(Factory& factory, std::ostream& os) {
    
    os << "; == LOADING RAMPS ==";
    os << "\n\n";
    for (auto ramp = factory.ramp_cbegin(); ramp != factory.ramp_cend(); ramp++) {
        os <<  "LOADING_RAMP id=" << ramp->get_id() << " delivery-interval=" << ramp->get_delivery_interval() << "\n";
    }
    os << "\n; == WORKERS ==";
    os << "\n\n";
    for (auto worker = factory.worker_cbegin(); worker != factory.worker_cend(); worker++) {
             os << "WORKER id=" << worker->get_id() << " processing-time=" << worker->get_processing_duration() << " queue-type="
            << en_to_string_queue(worker->get_queue()->get_queue_type()) << "\n";
    }

    os << "\n; == STOREHOUSES ==";
    os << "\n\n";

    for (auto store = factory.storehouse_cbegin(); store != factory.storehouse_cend(); store++) {
            os << "STOREHOUSE id=" << store->get_id() << "\n";
        }
    os << "\n; == LINKS ==";
    os << "\n\n";
    std::for_each(factory.ramp_cbegin(), factory.ramp_cend(), [&os](const auto& ramp) {
        std::vector<IPackageReceiver*> receivers = find_receivers(ramp);
        for (const auto& receiver: receivers){
            os << "LINK src=ramp-" << ramp.get_id() << " dest=" << en_to_string_receiver(receiver->get_receiver_type()) << receiver->get_id() << "\n";
        }
        os << "\n";
    });
    std::for_each(factory.worker_cbegin(), factory.worker_cend(), [&os](const auto& worker) {
        std::vector<IPackageReceiver*> receivers = find_receivers(worker);
        for (const auto& receiver: receivers){
            os << "LINK src=worker-" << worker.get_id() << " dest=" << en_to_string_receiver(receiver->get_receiver_type()) << receiver->get_id() << "\n";
        }
        os << "\n";
    });
    os.flush();
}


// funkcja zwraca wektor wskaźników odbiorców
std::vector<IPackageReceiver*> find_receivers(const PackageSender& sender){
    std::vector<IPackageReceiver*> receivers;
    for (const auto& pair: sender.receiver_preferences_){
        receivers.emplace_back(pair.first);
    }
    return receivers;
}


// funkcja zwracajaca typ kolejki - zmiana z wyliczenia na string
std::string en_to_string_queue(PackageQueueType type){
    switch(type){
        case PackageQueueType::FIFO:
            return "FIFO";
        case PackageQueueType::LIFO:
            return "LIFO";
        default:
            return "";
    }
}


// funkcja zwracajaca typ odbiorcy - zmiana z wyliczenia na string
std::string en_to_string_receiver(ReceiverType type){
    switch(type){
        case ReceiverType::Storehouse:
            return "store-";
        case ReceiverType::Worker:
            return "worker-";
        default:
            return "";
    }
}


ParsedLineData parse_line(std::string line){
    ParsedLineData parsedline;

    std::vector<std::string> tokens;
    std::string token;

    std::istringstream token_stream(line);
    char delimiter = ' ';

    while(std::getline(token_stream, token, delimiter)){
        tokens.push_back(token);
    }

    if(line.find("LOADING_RAMP") != std::string::npos){
        parsedline.element_type_ = ElementType::LOADING_RAMP;
    } else if (line.find("WORKER") != std::string::npos){
        parsedline.element_type_ = ElementType::WORKER;
    } else if (line.find("STOREHOUSE") != std::string::npos){
        parsedline.element_type_ = ElementType::STOREHOUSE;
    } else if (line.find("LINK") != std::string::npos){
        parsedline.element_type_ = ElementType::LINK;
    }
    for(auto token_it = tokens.begin()+1; token_it != tokens.cend(); token_it++){
        if(token_it->find("FIFO") != std::string::npos){
            parsedline.parameters["queue-type"] = "FIFO";
        } else if(token_it->find("LIFO") != std::string::npos){
            parsedline.parameters["queue-type"] = "LIFO";
        } else {
            std::size_t i = 0;
            for (; i < token_it->length(); i++) {
                if (isdigit((*token_it)[i])) {
                    break;
                }
            }
            std::string num = token_it->substr(i, token_it->length() - i);
            std::string key = token_it->substr(0, i - 1);
            parsedline.parameters[key] = num;
        }
    }
    return parsedline;
}