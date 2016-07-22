#pragma once
#include <vector>
#include <string>
#include <boost/algorithm/string.hpp>
#include "boost/predef.h"
#include "exceptions/exceptions.hpp"

using namespace std;

namespace spina {

static vector<string> tokeniseString(const string& message, const string& spchar) {
	vector<string> messageTokens;
	if (!message.empty()) {
		boost::split(messageTokens, message, boost::is_any_of(spchar));
	}

	return messageTokens;
};

}

// We have to do some icky things on Windows!
#if BOOST_OS_WINDOWS
	#if defined(MODULE_EXPORT)
		#define SPINA_WINEXPORT __declspec(dllexport)
	#else
		#define SPINA_WINEXPORT __declspec(dllimport)
	#endif
#else
	#define SPINA_WINEXPORT
#endif
