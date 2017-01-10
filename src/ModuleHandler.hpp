#pragma once
#include <dlfcn.h>
#include <string>
#include <set>
#include <atomic>
#include <boost/filesystem.hpp>

#include "Module.hpp"
#include "interfaces/logger.hpp"

using namespace spina::interfaces;

namespace spina {
	class ModuleHandler {
		private:
			const string name = "ModuleHandler";
			string _filename;
			Logger _logger;

			void* _module_so            = nullptr;
			Module_ctor*  createModule  = nullptr;
			Module_dctor* destroyModule = nullptr;

			inline bool openLibraryFile();
			inline int  loadLibrarySymbols();

		public:
			// This is public so Spina can use it nicely.
			Module* module = nullptr;
			const ModuleInfo* moduleInfo;
			
			atomic<bool> _moduleLoaded{false};
			atomic<bool> _moduleInit  {false};

			inline ModuleHandler(const string& filename) {_filename = filename;};
			//TODO: Deconstructor
			inline bool loadLibrary();
			inline void unloadLibrary();

			inline bool initModule();
			inline void deinitModule();

			inline bool isLoaded() const;
	};

	// const string ModuleHandler::name = "ModuleHandler";

	bool ModuleHandler::openLibraryFile() {
		// Here we use dlopen to load the dynamic library (that is, a compiled module)
		_module_so = dlopen(_filename.c_str(), RTLD_NOW | RTLD_GLOBAL);
		if (!_module_so) {
			_logger.err(name, dlerror());
			return false;
		}
		return true;
	};

	int ModuleHandler::loadLibrarySymbols() {
		// Here we use dlsym to hook into exported functions of the module
		// One to load the module class and one to unload it
		createModule = (Module_ctor*)dlsym(_module_so, "createModule");
		if (!createModule) {
			_logger.err(name, dlerror());
			return 1;
		}
		_logger.log(name, "Resolved loadModule", true);

		destroyModule = (Module_dctor*)dlsym(_module_so, "destroyModule");
		if (!destroyModule) {
			return 2;
		}
		_logger.log(name, "Resolved unloadModule", true);

		return 0;
	}

	bool ModuleHandler::loadLibrary() {
		// In the threads we load the binary and hook into it's exported functions.
		// We use the load function to create an instance of it's Module-derived class
		// We then set up an interface with the module using our input/output sockets
		//  which are a management Rep and Req socket for commands in and out
		//  and a chain-building pair of Pub and Sub sockets for message passing.
		// Now we run it's run() function in a new thread and push that on to our list.
		if (! openLibraryFile()) {
			_logger.err(name, "Failiure " + boost::filesystem::basename(_filename) + "..");
			return false;
		}
		_logger.log(name, "Opened file " + boost::filesystem::basename(_filename) + "..", true);

		if (loadLibrarySymbols() != 0) {
			_logger.err(name, "Failiure " + boost::filesystem::basename(_filename) + "..");
			return false;
		}
		_logger.log(name, "Functions resolved", true);

		_moduleLoaded.store(true);
		return _moduleLoaded;
	};

	void ModuleHandler::unloadLibrary() {
		if (_moduleInit.load()) {
			deinitModule();
		}

		if (dlclose(_module_so) != 0) {
			_logger.err(name, "Could not dlclose module file");
		}

		_moduleLoaded.store(false);
	};

	bool ModuleHandler::initModule() {
		if (!_moduleInit) {
			module = createModule();
			if (module) {
				moduleInfo = module->getinfo();
				_moduleInit.store(true);
			}
		}

		return _moduleInit.load();
	};

	void ModuleHandler::deinitModule() {
		if (_moduleInit.load() && module) {
			delete module;
			module = nullptr;
			_moduleInit.store(false);
		}
	};
}
