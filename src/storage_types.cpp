#include <storage_types.hpp>

Package PackageQueue::pop() {
    
    switch (get_queue_type()) {
        case PackageQueueType::FIFO: {
            Package apackage = Package(std::move(queue_.front()));
            queue_.pop_front();
            return apackage;
        }
        case PackageQueueType::LIFO: {
            Package apackage = Package(std::move(queue_.back()));
            queue_.pop_back();
            return apackage;
        }
        default : {
            Package apackage = Package(std::move(queue_.back()));
            queue_.pop_back();
            return apackage;
        }
    }
}


PackageQueue::PackageQueue(PackageQueueType type) {

    type_of_Queue_ = type;
}
