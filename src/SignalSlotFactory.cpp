#include "SignalSlotFactory.hpp"

namespace spina {

// Create
typename SignalSlotFactory::signal_ptr SignalSlotFactory::createSignal(const std::string& name) {
    signals[name] = std::shared_ptr<signal_t>(new signal_t);
    return signals[name];
};

void SignalSlotFactory::createSlot(const std::string& name, const SignalSlotFactory::slot_t subscriber) {
    slots.insert({ name, subscriber });
};


// Connect
typename SignalSlotFactory::conn_t SignalSlotFactory::connect(const std::string& name, const SignalSlotFactory::slot_t &subscriber, const int& index) {
    auto sig = assert_signal_exists(name);
    return sig->second->connect(index, subscriber);
};

typename SignalSlotFactory::conn_t SignalSlotFactory::connect(const std::string& signal_name, const std::string& slot_name, const int& index) {
    auto sl = assert_slot_exists(slot_name);
    return connect(signal_name, sl->second, index);
};


// Asserts
typename SignalSlotFactory::signal_map::iterator SignalSlotFactory::assert_signal_exists(const std::string& name) {
    auto sig = signals.find(name);
    if (sig == signals.end())
        throw 50;

    return sig;
};

typename SignalSlotFactory::slot_map::iterator SignalSlotFactory::assert_slot_exists(const std::string& name) {
    auto sig = slots.find(name);
    if (sig == slots.end())
        throw 50;

    return sig;
};

// Raise
void SignalSlotFactory::raise(const std::string& signal_name, const std::string& data) {
    auto sig = assert_signal_exists(signal_name);
    auto p = sig->second.get();
    (*p)(data);
};

}
