#include <boost/filesystem.hpp>
#include <boost/range/iterator_range_core.hpp>

#include "messages/messages.hpp"
#include "messages/socketer.hpp"
#include "exceptions/exceptions.hpp"
#include "modulecom.hpp"

using namespace spina::messages;
using namespace spina::exceptions::spina;
using namespace spina::exceptions::uri;

#include "spina/spina.hpp"

namespace spina {

#if BOOST_OS_MACOS
 	const string Spina::moduleFileExtension = ".dylib";
#else
	#if BOOST_OS_WINDOWS
		const string Spina::moduleFileExtension = ".dll";
	#else // Linux
		const string Spina::moduleFileExtension = ".so";
	#endif
#endif

Spina::Spina() : Module("spina", "Joe Eaves", false) {
	//TODO: Is there a lot of stuff that could throw here? It needs looking at
	setContext(Context::getSingleContext());
	sigslot.createSignal("registered");

	_running.store(true);

	// Hack to die after so long while we're in dev.
	// _eventer->on("close-timeout", [&](chrono::milliseconds) {
	// 	_running.store(false);
	// }, chrono::milliseconds(60000), EventPriority::HIGH);
}

Spina::~Spina() {
	_running.store(false);

	for_each(m_threads.begin(), m_threads.end(), [&](thread& t) {
		if (t.joinable()) {
			t.join();
			_logger.log(name(), "Joined", true);
		};
	});

	_logger.log(name(), "Closed");
}

bool Spina::isRunning() {
	return _running.load();
}

void Spina::tick(){
	_eventer->emitTimedEvents();
};

bool Spina::registerModule(const string& modName) {
	loadedModules.push_back(modName);
	_logger.log(name(), "Registered module: " + modName + "!");
	return true;
};

bool Spina::unregisterModule(const string& modName) {
	loadedModules.erase(std::remove(loadedModules.begin(), loadedModules.end(), modName), loadedModules.end());
	_logger.log(name(), "Unregistered module: " + modName + "!");
	return true;
};

bool Spina::loadModule(const string& filename, const int& index) {
	// Strart a new thread holding a modulecom.
	// The com will load the module from disk and then re-init it while it's loaded.
	// We can add logic for unloading it later
	// For now we've added a check on the atomic _running bool, so the Spina can tell us to unload,
	//   so as to close the module and make the thread join()able.
	m_threads.push_back(thread([&,filename,index] {

		shared_ptr<ModuleCOM> com(new ModuleCOM(filename));
		_logger.log(name(), "Loading " + boost::filesystem::basename(filename) + "...", true);

		com->loadLibrary();

		if (com->initModule(_socketer->getContext()) && _running.load()) {
			try {
				auto modname = com->module->name();
				string modaction = modname + "-action";
				cout << modaction << endl;

				// Registration
				SignalSlotFactory::conn_t conreg;
				sigslot.createSlot(modname + "-registered", [&] (const string& data) {
					_logger.log("[SLOT]", "Registration for " + modname + " with " + data);
					registerModule(modname);
					conreg.disconnect();
				});
				conreg = sigslot.connect("registered", modname + "-registered", index);

				// Actions
				sigslot.createSignal(modname);

				SignalSlotFactory::slot_t action_slot( [&] (const string& action) {
					_logger.log("[SLOT]", "Action " + action);
				});
				sigslot.createSlot(modaction, action_slot.track_foreign(com));
				sigslot.connect(modname, modaction);

				this_thread::sleep_for(chrono::seconds(2));
				sigslot.raise("registered", "HOPEFULLY SLICK");

				while (_running.load()) {
					if (com->isLoaded()) {
						try {
							this_thread::sleep_for(chrono::seconds(10));
						} catch (exception& e) {
							_logger.err(name(), string("CAUGHT POLLTICK OF MODULE ") + modname + " " + e.what());
						}
					} else {
						break;
					}
				}
			} catch (const std::exception& ex) {
				cout << ex.what() << endl;
			}

			// Here the module is essentially dead, but not the thread
			//TODO: Module reloading code, proper shutdown handlers etc.
			com->deinitModule();
			// unregisterModule(com.moduleName);
		}

		// If we get here, be joinable so the thread is ready for reaping!
		this_thread::sleep_for(chrono::milliseconds(1000));
		com->unloadLibrary();
	}));

	return true;
}

bool Spina::loadModules(const string& directory) {
	// Here we gather a list of relevant module binaries
	//  and then call the load function on each path
	set<string> moduleFiles;
	try {
		listModuleFiles(moduleFiles, directory);
	} catch (InvalidModulePath& e) {
		_logger.log(name(), e.what(), true);
		return false;
	}

	_logger.log(name(), directory, true);

	int index = 0;
	for (auto filename : moduleFiles)
	{
		//TODO: We should check error messages here better!
		loadModule(filename, index);
		index++;
	}

	return true;
}

bool Spina::isModuleLoaded(std::string moduleName) {
	return std::find(loadedModules.begin(), loadedModules.end(), moduleName) != loadedModules.end();
}

set<string> Spina::listModuleFiles(const string& directory) {
	set<string> moduleFiles;

	Spina::listModuleFiles(moduleFiles, directory);

	return moduleFiles;
}

void Spina::listModuleFiles(set<string>& destination, const string& directory) {
	// Here we list a directory and build a vector of files that match
	//  our platform specific dynamic lib extension (e.g., .so)
	try {
		boost::filesystem::recursive_directory_iterator startd(directory), endd;
		auto files = boost::make_iterator_range(startd, endd);

		for(boost::filesystem::path p : files)
			if (p.extension() == Spina::moduleFileExtension && !boost::filesystem::is_directory(p))
				destination.insert(p.string());

	} catch(boost::filesystem::filesystem_error& e) {
		throw InvalidModulePath(directory, static_cast<string>(e.what()));
	}

}


}

spina::Spina* createModule(){return new spina::Spina;}
void destroyModule(spina::Spina* module) {delete module;}
