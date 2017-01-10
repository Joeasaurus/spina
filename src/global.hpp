#pragma once
#include <vector>
#include <string>
#include "boost/predef.h"
#include "exceptions/exceptions.hpp"

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
