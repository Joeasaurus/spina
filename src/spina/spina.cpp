#include <boost/filesystem.hpp>
#include <boost/range/iterator_range_core.hpp>

#include "exceptions/exceptions.hpp"
#include "ModuleHandler.hpp"

using namespace spina::exceptions::spina;

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

Spina::Spina() : Module("spina", "Joe Eaves") {

	_running.store(true);

	// Hack to die after so long while we're in dev.
	// _eventer->on("close-timeout", [&](chrono::milliseconds) {
	// 	_running.store(false);
	// }, chrono::milliseconds(60000), EventPriority::HIGH);
}

Spina::~Spina() {
	_running.store(false);
	_logger.log(info.name, "Closed");
}

bool Spina::isRunning() {
	return _running.load();
}

bool Spina::registerModule(unique_ptr<ModuleHandler>& handler) {
    _logger.log(info.name, "Registering module: " + handler->moduleInfo->name + "!");
	return loadedModules.emplace(handler->moduleInfo->name, std::move(handler)).second;
};

bool Spina::unregisterModule(const string& modName) {
	loadedModules.erase(modName); //TODO: LEAKY HERE
	_logger.log(info.name, "Unregistered module: " + modName + "!");
	return true;
};

bool Spina::loadModule(const string& filename) {

    unique_ptr<ModuleHandler> handler(new ModuleHandler(filename));

	_logger.log(info.name, "Loading " + boost::filesystem::basename(filename) + "...", true);

    handler->loadLibrary();
    handler->initModule();

    // handler->slotSignals(sigslot);

    return registerModule(handler);
}

bool Spina::loadModules(const string& directory) {
	// Here we gather a list of relevant module binaries
	//  and then call the load function on each path
	set<string> moduleFiles;
	try {
		listModuleFiles(moduleFiles, directory);
	} catch (InvalidModulePath& e) {
		_logger.log(info.name, e.what(), true);
		return false;
	}

	_logger.log(info.name, directory, true);

	for (auto filename : moduleFiles) {
		//TODO: We should check error messages here better!
		loadModule(filename);
	}

	return loadedModules.size() > 0;
}

bool Spina::isModuleLoaded(std::string moduleName) {
	return loadedModules.find(moduleName) != loadedModules.end();
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
