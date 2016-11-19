#pragma once

#include <string>
#include <map>
#include <functional>
#include <boost/signals2/signal.hpp>

namespace spina {

class SignalSlotFactory {
public:

    using conn_t     = boost::signals2::connection;
    using block_t    = boost::signals2::shared_connection_block;
    using signal_t   = boost::signals2::signal<void(std::string)>;
    using signal_ptr = std::shared_ptr<signal_t>;
    using signal_map = std::map<std::string, signal_ptr>;
    using slot_t     = signal_t::slot_type;
    using slot_ptr   = std::shared_ptr<typename signal_t::slot_type>;
    using slot_map   = std::map<std::string, slot_t>;

    signal_ptr createSignal(const std::string& name);
    void createSlot(const std::string& name, const slot_t subscriber);
    conn_t connect(const std::string& signal_name, const std::string& slot_name, const int& index = 0);
    conn_t connect(const std::string& signal_name, const typename signal_t::slot_type &subscriber, const int& index = 0);

    void raise(const std::string& signal_name, const std::string& data);

private:
    slot_map   slots;
    signal_map signals;
    signal_map::iterator assert_signal_exists(const std::string& name);
    slot_map::iterator assert_slot_exists(const std::string& name);
};

}
