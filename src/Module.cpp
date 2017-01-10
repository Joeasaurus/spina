#include "Module.hpp"

namespace spina {

Module::Module(string name, string author) {
    timeNow = chrono::system_clock::now();
    info.name = name;
    info.author = author;
}

std::string Module::name() const {
    return info.name;
}

const ModuleInfo* Module::getinfo() const {
    return &info;
}

}
