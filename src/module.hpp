#pragma once

#include <string>
#include <chrono>
#include <functional>
#include <cppevent/Eventer.hpp>

#include "global.hpp"
#include "interfaces/logger.hpp"
#include "messages/socketer.hpp"
#include "messages/messages.hpp"
#include "uri/urirouter.hpp"

using namespace std;
using namespace cppevent;

namespace spina {

	typedef struct ModuleInfo {
	    string name = "undefined module";
	    string author = "mainline";
	} ModuleInfo;

	class ModuleCOM;

	using namespace messages;

	class Module {
		friend class ModuleCOM;
		private:
			chrono::system_clock::time_point timeNow;
			void serviceHooks_Registration();

		protected:
			Socketer*  _socketer = nullptr;
			Eventer*   _eventer  = nullptr;
			Logger     _logger;
			ModuleInfo __info;
			URIRouter router;

			atomic<bool> _withHooks {true};
			atomic<bool> registered {false};

		public:
			Module(string name, string author, bool withHooks = true);
			virtual ~Module();

			virtual void polltick();
			virtual void tick(){};
			virtual void setup()=0;

			string name() const;
			void connectToParent(string parent, const Context& ctx);
	};
}

/* Export the module
 *
 * These functions should be overriden and set 'export "C"' on.
 * These functions allow us to load the module dynamically via <dlfcn.h>
 */
typedef SPINA_WINEXPORT spina::Module* Module_ctor(void);
typedef SPINA_WINEXPORT void Module_dctor(spina::Module*);
