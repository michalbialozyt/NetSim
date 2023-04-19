#include <nodes.hpp>

void ReceiverPreferences::add_receiver(IPackageReceiver* r){

    if (preferences_t_.size() == 0){
        preferences_t_.insert(std::pair<IPackageReceiver*, double>(r, 1));
    }else{
        double size = static_cast<float>(preferences_t_.size());
        preferences_t_.emplace(std::pair<IPackageReceiver*, double>(r, 1/(size)));
    }

    double sum_of_probability = 0;
    for(std::map<IPackageReceiver*, double>::iterator it = std::begin(preferences_t_); it!=std::end(preferences_t_); ++it){
        sum_of_probability += it->second;
    }

    std::map<IPackageReceiver*, double>::iterator iter;
    for (iter = std::begin(preferences_t_); iter != std::end(preferences_t_); iter++){
        iter->second = (iter->second)/sum_of_probability;
    }
}

void ReceiverPreferences::remove_receiver(IPackageReceiver* r) {

    auto iter = preferences_t_.find(r);
    if (iter != std::end(preferences_t_)){
        preferences_t_.erase(iter);
    }

    double sum_of_probability = 0;
    for(std::map<IPackageReceiver*, double>::iterator it = std::begin(preferences_t_); it!=std::end(preferences_t_); ++it){
        sum_of_probability += it->second;
    }

    std::map<IPackageReceiver*, double>::iterator iter1;
    for (iter1 = std::begin(preferences_t_); iter1 != std::end(preferences_t_); iter1++){
        iter1->second = (iter1->second)/sum_of_probability;
    }
}

IPackageReceiver* ReceiverPreferences::choose_receiver() {

    std::vector<std::pair<IPackageReceiver*, double>> vec;

    double pg = pg_();

    std::copy(begin(), end(),
              std::back_inserter<std::vector<std::pair<IPackageReceiver*, double>>>(vec));

    double probability_sum = 0;
    for (auto& pair: vec){
        probability_sum += pair.second;
        pair.second = probability_sum;
    }
    IPackageReceiver* receiver = vec[0].first;
    for (const auto& pair: vec) {
        if (pg <= pair.second){
            receiver = pair.first;
            break;
        }
    }
    return receiver;
}

void Worker::do_work(Time t) {

    if(!buffer_p_.has_value()) {
        if(!q_->empty()){
            buffer_p_.emplace(q_->pop());
            start_t_ = t;
            if( pd_ - 1 <=  t - start_t_){
                buffer_s_.emplace(std::move(buffer_p_.value()));
                buffer_p_.reset();
            }
        }
    }else{
        if( pd_ - 1 <=  t - start_t_){
            buffer_s_.emplace(std::move(buffer_p_.value()));
            buffer_p_.reset();
        }
    }


}

void PackageSender::push_package(Package&& package) {
    buffer_s_.emplace(std::move(package));
}


void Worker::receive_package(Package&& p) {
    q_->push(std::move(p));
}

void PackageSender::send_package() {
    if (buffer_s_){

        receiver_ptr_ = receiver_preferences_.choose_receiver();
        receiver_ptr_->receive_package(buffer_s_.value().get_id());
        buffer_s_.reset();
    }
}


void Ramp::deliver_goods(Time t) {

    if (!((t - 1) % di_)){
        push_package(Package());
    }
}