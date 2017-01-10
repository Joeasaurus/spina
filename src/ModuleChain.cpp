#include "ModuleChain.hpp"
#include <algorithm>
#include "interfaces/logger.hpp"

using namespace std;

namespace spina {

void ModuleChain::addModule(Module* module) {
    auto m_name = module->name();
    if (modules.find(m_name) == modules.end())
        modules[m_name] = module;
}

vector<ModuleOrder> ModuleChain::getModuleOrder() const {
    return module_order;
}

void ModuleChain::setModuleOrder(const vector<ModuleOrder>& new_order) {
    module_order = new_order;
}

void ModuleChain::run() {
    boost::signals2::signal<bool ()> sig;
    interfaces::Logger logger;

    for (auto& morder : module_order) {
        logger.log("chain", morder.name);
        auto module = modules.at(morder.name);
        auto module_run = [&,module] {
            logger.log("chain", "CALLED " + module->name());
            return module->run(&global_state);
        };

        if (morder.orders.size() == 0) {
            logger.log("chain", "defaulting to 0");
            sig.connect(module_run);
        } else {
            for (auto& order : morder.orders) {
                logger.log("chain", "setting order");
                sig.connect(order, module_run);
            }
        }
    }

    auto x = sig();

}


}
