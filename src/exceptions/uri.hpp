#pragma once
#include "exceptions/general.hpp"
using namespace std;
using namespace spina::exceptions::general;

namespace spina { namespace exceptions { namespace uri {

	class ParamNotFound: public GeneralFailure {
		public:

			ParamNotFound(const string& param) : GeneralFailure( "ParamNotFound" ) {
				message = param;
			};
	};
}}}
