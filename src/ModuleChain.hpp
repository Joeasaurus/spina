#pragma once
#include <string>
#include <vector>
#include <map>
#include <boost/signals2.hpp>

#include "Module.hpp"
#include "ModuleDataMap.hpp"

namespace spina {

struct ModuleOrder {
    std::string name;
    vector<int> orders;
};

class ModuleChain {
    ModuleDataMap global_state;
    map<std::string, Module*> modules;
    vector<ModuleOrder> module_order;

public:
    void addModule(Module* module);
    vector<ModuleOrder> getModuleOrder() const;
    void setModuleOrder(const vector<ModuleOrder>& new_order);
    void run();
};

}
