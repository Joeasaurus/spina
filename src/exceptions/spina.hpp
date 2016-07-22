#pragma once
#include "exceptions/general.hpp"
using namespace std;
using namespace spina::exceptions::general;

namespace spina { namespace exceptions { namespace spina {

	class InvalidModulePath: public GeneralFailure {
		public:
			InvalidModulePath(const string& dir, const string& msg) : GeneralFailure("InvalidModulePath") {
				path = dir;
				message = msg;
			};

		private:
			string path;
	};
}}}
