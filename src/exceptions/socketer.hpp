#pragma once
#include "exceptions/general.hpp"
using namespace std;
using namespace spina::exceptions::general;

namespace spina { namespace exceptions { namespace socketer {

	class NonExistantHook: public GeneralFailure {
		public:
			NonExistantHook(const string& hook) : GeneralFailure("NonExistantHook") {
				message = hook;
			};

			bool isCritical() const SPINA_CLANGNOEXCEPT {
				if (message == "process_command" || message == "process_input")
					return true;
				return false;
			}
	};
}}}
