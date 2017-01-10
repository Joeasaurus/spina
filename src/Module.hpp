#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <chrono>
#include <functional>

#include "global.hpp"
#include "ModuleData.hpp"
#include "ModuleDataMap.hpp"
#include "interfaces/logger.hpp"

using namespace std;

namespace spina {

	typedef struct ModuleInfo {
	    string name   = "BASE CLASS MODULE";
	    string author = "BASE CLASS AUTHOR";
		ModuleDataMap mdmap;
	} ModuleInfo;

	class ModuleHandler;
	class Module {
		private:
			chrono::system_clock::time_point timeNow;

		protected:
			interfaces::Logger _logger;
			ModuleInfo info;

		public:
			Module(string name, string author);
			virtual ~Module(){};

			string name() const;
			const ModuleInfo* getinfo() const;

			virtual bool run(ModuleDataMap* global_state)=0;
	};
}

/* Export the module
 *
 * These functions should be overriden and set 'export "C"' on.
 * These functions allow us to load the module dynamically via <dlfcn.h>
 */
typedef SPINA_WINEXPORT spina::Module* Module_ctor(void);
typedef SPINA_WINEXPORT void Module_dctor(spina::Module*);
