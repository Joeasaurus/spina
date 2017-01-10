#include "ModuleDataMap.hpp"

namespace spina {

void ModuleDataMap::publish(const std::string& name, std::shared_ptr<ModuleData> newptr) {
    lock_guard<mutex> lock(mod_data_mutex);
    mod_data_map[name] = newptr;
}

void ModuleDataMap::unpublish(const std::string& name) {
    lock_guard<mutex> lock(mod_data_mutex);
    mod_data_map.erase(name);
}

std::shared_ptr<ModuleData> ModuleDataMap::get(const std::string& name)  {
    lock_guard<mutex> lock(mod_data_mutex);
    return mod_data_map.at(name);
}

}
