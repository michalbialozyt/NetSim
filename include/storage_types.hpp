#ifndef NET_SIMULATION_STORAGE_TYPES_HPP
#define NET_SIMULATION_STORAGE_TYPES_HPP

#include <list>
#include <package.hpp>


enum class PackageQueueType{
    FIFO,
    LIFO
};


class IPackageStockpile{

public:
    using const_iterator = std::list<Package>::const_iterator;
    virtual void push(Package&& package) = 0;
    virtual int size() const = 0;
    virtual bool empty() const = 0;

    virtual const_iterator end() const = 0;
    virtual const_iterator cbegin() const = 0;
    virtual const_iterator cend() const = 0;
    virtual const_iterator begin() const = 0;

    virtual ~IPackageStockpile() {}
};


class IPackageQueue: public IPackageStockpile{

public:
    virtual PackageQueueType get_queue_type() const = 0;
    virtual Package pop() = 0;
    virtual ~IPackageQueue() override {}
};


class PackageQueue: public IPackageQueue{

public:
    PackageQueue(PackageQueueType type);

    void push(Package&& semi_package) override { queue_.emplace_back(std::move(semi_package)); }
    PackageQueueType get_queue_type() const override { return type_of_Queue_;}
    Package pop() override;
    int size() const override { return queue_.size();}
    bool empty() const override { return queue_.size() == 0;}


    IPackageStockpile::const_iterator cbegin() const override { return queue_.cbegin(); }
    IPackageStockpile::const_iterator cend() const override { return queue_.cend(); }
    IPackageStockpile::const_iterator begin() const override { return queue_.cbegin(); }
    IPackageStockpile::const_iterator end() const override { return queue_.cend(); }

private:
    std::list<Package> queue_;
    PackageQueueType type_of_Queue_;
};
#endif //NET_SIMULATION_STORAGE_TYPES_HPP
