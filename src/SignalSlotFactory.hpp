#pragma once

#include <string>
#include <map>
#include <functional>
#include <boost/signals2/signal.hpp>

namespace spina {

class SignalSlotFactory {
public:

    using conn_t      = boost::signals2::connection;
    using block_t     = boost::signals2::shared_connection_block;
    using signal_tv   = boost::signals2::signal<void()>;
    using signal_ts   = boost::signals2::signal<void(std::string)>;
    using signal_ptrv = std::shared_ptr<signal_tv>;
    using signal_ptrs = std::shared_ptr<signal_ts>;
    using signal_mapv = std::map<std::string, signal_ptrv>;
    using signal_maps = std::map<std::string, signal_ptrs>;
    using slot_ptrv   = std::shared_ptr<typename signal_tv::slot_type>;
    using slot_ptrs   = std::shared_ptr<typename signal_ts::slot_type>;
    using slot_mapv   = std::map<std::string, signal_tv::slot_type>;
    using slot_maps   = std::map<std::string, signal_ts::slot_type>;

    virtual void createSignal()=0;
    virtual void createSlot()=0;
    virtual void raise()=0;

    void createSignal(const std::string& name);
    void createSlot  (const std::string& name, const signal_tv::slot_type subscriber);
    void createSlot  (const std::string& name, const signal_ts::slot_type subscriber);

    void raise(const std::string& signal_name);
    void raise(const std::string& signal_name, const std::string& data);

    conn_t connect(const std::string& signal_name, const std::string& slot_name, const int& index = 0);

private:
    slot_mapv   vslots;
    slot_maps   sslots;
    signal_mapv vsignals;
    signal_maps ssignals;
    signal_mapv::iterator assert_vsignal_exists(const std::string& name);
    signal_maps::iterator assert_ssignal_exists(const std::string& name);
    slot_mapv::iterator   assert_vslot_exists(const std::string& name);
    slot_maps::iterator   assert_sslot_exists(const std::string& name);
};

}
