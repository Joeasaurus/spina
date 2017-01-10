#include "spina/spina.hpp"
#include "ModuleChain.hpp"
#include "interfaces/logger.hpp"
#include <iostream>
#include <thread>
#include <chrono>

/* NOTES
 * To save space in modules, spina should provide functions for loading files?
 *	(So boost-filesystem + system only need linking once)
 * Change module loading so we use author + name instead of relying on the filename
 */

using namespace spina;
using namespace spina::interfaces;
using namespace std;

int main(int argc, char **argv) {

	Logger logger;
	logger.setDebug(true);
	string modPath;

	// This is a quick fix for debug logging by managing argv ourselves
	// We'll move to an option parser later
	if (argc > 1) {
		modPath = argv[1];
	} else {
		logger.err("[Main]", "You forgot to supply a modules directory!");
		this_thread::sleep_for(chrono::milliseconds(10));
		return 1;
	}

	Spina spina;
	ModuleChain chain;
	spina.loadModules(modPath);
	for (auto& module : spina.loadedModules) {
		chain.addModule(module.second->module);
	}

	chain.setModuleOrder({
		{"output", {1}},
		{"input", {0}}
	});

	while (spina.isRunning()) {
		try {
			chain.run();
			this_thread::sleep_for(chrono::milliseconds(5000));
		} catch (exception& e) {
			logger.err("Main", string("CAUGHT POLLTICK OF SPINE ") + e.what());
			this_thread::sleep_for(chrono::milliseconds(10));
		}
	}
	return 0;
}
