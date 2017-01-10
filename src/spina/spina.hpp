#pragma once
#include <set>
#include <map>
#include <string>
#include <dlfcn.h>
#include <mutex>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <vector>
#include <future>

#include "Module.hpp"
#include "ModuleHandler.hpp"
#include "ModuleDataMap.hpp"

using namespace std;

namespace spina {

	class Spina : public Module {
	private:
		atomic<bool> _running{false};
		vector<thread> _threads;
		ModuleDataMap m_datamap;

		bool isModuleFile(const string& filename);

	public:
		static const string moduleFileExtension;
		map<string, unique_ptr<ModuleHandler>> loadedModules;

		Spina();
		~Spina();

		bool loadModules(const string& directory);
		bool loadModule(const string& filename);

		bool isModuleLoaded(std::string moduleName);
		bool isRunning();

		bool registerModule(unique_ptr<ModuleHandler>& handler);
		bool unregisterModule(const string& name);

		static set<string> listModuleFiles(const string& directory);
		static void listModuleFiles(set<string>& destination, const string& directory);

		inline bool run(ModuleDataMap* global_state){return true;};
	};
}

// Init/Del functions.
extern "C" SPINA_WINEXPORT spina::Spina* createModule();
extern "C" SPINA_WINEXPORT void destroyModule(spina::Spina* module);

/*
 * The spina shall be a HUB for all messages between modules.
 * Modules are black-box I/O units. Messages go in and messages come out
 * A Module (the spina included) shall have two sockets: PUB & SUB
 * Each module shall be connected in a spina-centric star topology
 * There shall be three channels for messages between a module and the spina:
 *   - 'COMMAND' This is for control messages between the two, e.g. config changes
 *   - 'INPUT' This is data from spina to be processed by module
 *   - 'OUTPUT' This is data from module to be handled by spina
 * The spina shall be responsible for managing logical chains of modules
 * The spina will route OUTPUT messages from moduleA to INPUT of moduleB
 */
