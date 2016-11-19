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

#include "Chain.hpp"
#include "ChainFactory.hpp"
#include "Module.hpp"
#include "SignalSlotFactory.hpp"

using namespace cppevent;
using namespace spina::messages;

namespace spina {

	class Spina : public Module {
	private:
		atomic<bool> _running{false};
		vector<thread> m_threads;
		mutex _moduleRegisterMutex; // Protects loadedModules

		bool isModuleFile(const string& filename);

	public:
		static const string moduleFileExtension;
		SignalSlotFactory sigslot;
		vector<string> loadedModules;

		Spina();
		~Spina();
		void setup(){};
		void tick();

		bool loadModules(const string& directory);
		bool loadModule(const string& filename, const int& index = 0);

		bool isModuleLoaded(std::string moduleName);
		bool isRunning();

		bool registerModule(const string& name);
		bool unregisterModule(const string& name);

		static set<string> listModuleFiles(const string& directory);
		static void listModuleFiles(set<string>& destination, const string& directory);
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
