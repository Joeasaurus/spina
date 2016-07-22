

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

Spina::Spina() : Module("spina", "Joe Eaves", false) {
	//TODO: Is there a lot of stuff that could throw here? It needs looking at
	connectToParent("__bind__", Context::getSingleContext());

	hookSocketCommands();

	auto index = chainFactory.create();
	chainFactory.insert(index, "input");
	chainFactory.insert(index, "input");
	assert(index == 1);

	list<unsigned long> refList{index};
	authoredChains["output"] = refList;

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


set<string> Spina::listModuleFiles(const string& directory) const {
	set<string> moduleFiles;

	listModuleFiles(moduleFiles, directory);

	return moduleFiles;
}

void Spina::listModuleFiles(set<string>& destination, const string& directory) const {
	// Here we list a directory and build a vector of files that match
	//  our platform specific dynamic lib extension (e.g., .so)
	try {
		boost::filesystem::recursive_directory_iterator startd(directory), endd;
		auto files = boost::make_iterator_range(startd, endd);

		for(boost::filesystem::path p : files)
			if (p.extension() == this->moduleFileExtension && !boost::filesystem::is_directory(p))
				destination.insert(p.string());

	} catch(boost::filesystem::filesystem_error& e) {
		_logger.warn(name(), "WARNING! Could not load modules!");
		throw InvalidModulePath(directory, static_cast<string>(e.what()));
	}

}

bool Spina::registerModule(const string& modName) {
	_loadedModules.insert(modName);
	_logger.log(name(), "Registered module: " + modName + "!");
	return true;
};

bool Spina::unregisterModule(const string& modName) {
	_loadedModules.erase(modName);
	_logger.log(name(), "Unregistered module: " + modName + "!");
	return true;
};

bool Spina::loadModule(const string& filename) {
	// Strart a new thread holding a modulecom.
	// The com will load the module from disk and then re-init it while it's loaded.
	// We can add logic for unloading it later
	// For now we've added a check on the atomic _running bool, so the Spina can tell us to unload,
	//   so as to close the module and make the thread join()able.
	m_threads.push_back(thread([&,filename] {
		ModuleCOM com(filename);
		_logger.log(name(), "Loading " + boost::filesystem::basename(filename) + "...", true);

		if (com.loadLibrary()) {
			if (com.initModule(name(), _socketer->getContext()) && _running.load()) {
				try {
					com.module->setup();
					while (_running.load()) {
						if (com.isLoaded()) {
							try {
								com.module->polltick();
							} catch (exception& e) {
								_logger.err(name(), string("CAUGHT POLLTICK OF MODULE ") + com.module->name() + " " + e.what());
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
				com.deinitModule();
				// unregisterModule(com.moduleName);
			}

			this_thread::sleep_for(chrono::milliseconds(1000));
			com.unloadLibrary();
		}

		// If we get here the thread is joinable so it's ready for reaping!
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

	for (auto filename : moduleFiles)
	{
		//TODO: We should check error messages here better!
		loadModule(filename);
	}

	return true;
}

bool Spina::isModuleLoaded(std::string moduleName) {
	return this->_loadedModules.find(moduleName) != this->_loadedModules.end();
}

set<string> Spina::loadedModules() {
	return _loadedModules;
}




void Spina::hookSocketCommands() {
	router.on("module-loaded", MUri("pidel://spina/module/load/:modulename"), [&](MUri& mu, URIRouter* _rout) {
		auto modname = _rout->getVar("modulename");
		if (registerModule(modname)) {
			mu.command("/module/loaded/true");
		} else {
			mu.command("/module/loaded/false");
		}
		mu.send(_socketer, name(), modname, CHANNEL::Cmd);
	});
	// uriR.addRoute(modLoaded, [&](MUri& mu, map<string, string>& variables) {
	// 	command_moduleLoaded(mu, variables);
	// });

	// _socketer->on("process_command", [&](const Message& msg) {
	// 	MUri mu(msg.payload());
	// 	router.emit(mu);
	//
	// 	// _logger.log(name(), "CMD HEARD " + msg.payload() + " for " + mu.scheme());
	//
	// 	// if (mu.scheme() == name()) {
	// 	// 	// _logger.log(name(), "Command " + mu.getUri() + " was for me!");
	// 	// 	handleCommand(mu);
	// 	// } else {
	// 	// 	Message newmsg(msg.serialise(), false);
	// 	// 	newmsg.sendTo(mu.scheme());
	// 	// 	_socketer->sendMessage(newmsg);
	// 	// }
	//
	// 	return true;
	// });

	_socketer->on("process_input", [&](const Message& msg) {
		// _logger.log(name(), "INPUT HEARD " + msg.payload(), true);
		return true;
	});

	_socketer->on("process_output", [&](const Message& msg) {
		return handleOutput(msg);
	});
}

void Spina::handleCommand(MUri& mu) {
	try {
		if (mu.command() == "loaded") {
			auto param = mu.param("name");
			// command_moduleLoaded(param.front());
		}
	} catch (ParamNotFound& e) {
		_logger.err(name(), e.what());
	}
}

bool Spina::handleOutput(const Message& msg) {
	// HERE ENSUES THE ROUTING
	// The spina manages chains of modules, so we forward from out to in down the chains
	// _logger.log(name(), "OUTPUT HEARD " + msg.serialise(), true);

	auto modChain = msg.getChain();

	// Channel has already told us it's output to be routed
	// Output is never directed, input is!
	// If chainID == 0, get list of author chains for message author/sender
	// Create refs for each of those chains
	// Set the chain on the msg and send to the current() on it
	// If the chain next() is null, kill it

	Message inmsg(msg.m_from);
	inmsg.setChannel(CHANNEL::In);
	inmsg.payload(msg.payload());

	if (modChain.first == 0) {
		// _logger.log(name(), "Creating chains for " + msg.m_from, true);

		auto chains = authoredChains[msg.m_from];

		for (auto& chain : chains) {
			auto ref = chainFactory.create(chain);
			// _logger.log(name(), "... created " + to_string(chain) + "," + to_string(ref) + " ... with current() => " + chainFactory.current(chain, ref), true);

			inmsg.setChain(chain, ref);
			inmsg.sendTo(chainFactory.current(chain, ref));

			chainFactory.next(chain, ref);
			chainFactory.hasEnded(chain, ref, true); // kill it!

			_socketer->sendMessage(inmsg);
		}
	} else if (chainFactory.has(modChain.first, modChain.second)) {
		inmsg.sendTo(chainFactory.current(modChain.first, modChain.second));

		chainFactory.next(modChain.first, modChain.second);
		chainFactory.hasEnded(modChain.first, modChain.second, true); // kill it!

		_socketer->sendMessage(inmsg);
	} else {
		// _logger.log(name(), "DEAD message: " + msg.serialise());
		return false;
	}

	return true;
}

}

spina::Spina* createModule(){return new spina::Spina;}
void destroyModule(spina::Spina* module) {delete module;}
