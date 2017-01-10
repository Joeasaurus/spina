#pragma once

#include <mutex>
#include <map>
#include "ModuleData.hpp"

namespace spina {

class ModuleDataMap {
    std::mutex mod_data_mutex;
    map<std::string, std::shared_ptr<ModuleData>> mod_data_map;

public:
    void publish(const std::string& name, std::shared_ptr<ModuleData> newptr);
    void unpublish(const std::string& name);

    std::shared_ptr<ModuleData> get(const std::string& name);

    template<typename T>
    std::shared_ptr<T> getAs(const std::string& name);
};

}
