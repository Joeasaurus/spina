#include "spina/spina.hpp"
#include "interfaces/logger.hpp"
#include <iostream>

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
	string modPath = "NONE";

	// This is a quick fix for debug logging by managing argv ourselves
	// We'll move to an option parser later
	if (argc > 1) {
		modPath = argv[1];
	}

	if (modPath == "NONE") {
		logger.err("[Main]", "You forgot to supply a modules directory!");
		return 1;
	}

	Spina spina;



	if (spina.loadModules(modPath)) {
		while (true) {
			try {
				// spina.polltick();
				// this_thread::sleep_for(chrono::seconds(10));
				this_thread::sleep_for(chrono::seconds(5));
				spina.sigslot.raise("input", "I told you to do something!");
				for (int i = 0; i < spina.loadedModules.size(); i++) {
					logger.log("Main", spina.loadedModules[i] + " is loaded");
				}
				this_thread::sleep_for(chrono::seconds(5));
			} catch (exception& e) {
				logger.err("Main", string("CAUGHT POLLTICK OF SPINE ") + e.what());
			}
		}
		return 0;
	}

	return 1;
}
