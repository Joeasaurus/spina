#pragma once
#include "boost/predef.h"

// VS2-013 doesn't have noexcept!
#if BOOST_OS_WINDOWS
	#define SPINA_CLANGNOEXCEPT
#else
	#define SPINA_CLANGNOEXCEPT noexcept
#endif

#include <iostream>
#include <exception>
#include <stdexcept>
#include <sstream>
using namespace std;

namespace spina { namespace exceptions { namespace general {

	class GeneralFailure: public runtime_error {
		public:

			GeneralFailure(const string& msg = "GeneralFailure") : runtime_error(msg) {};

			virtual const char* what() const SPINA_CLANGNOEXCEPT {
				ostringstream cnvt;
				cnvt.str("");

				cnvt << runtime_error::what() << ": " << message;

				return strdup(cnvt.str().c_str());
			}
		protected:
			string message = "GeneralFailure";
	};
}}}
