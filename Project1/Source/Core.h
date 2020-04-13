#pragma once

// definition to import all the functions into dll

#ifdef GEAR_PLATFORM_WINDOWS
	#ifdef GEAR_BUILD_DLL
		#define GEAR_API __declspec(dllexport)
	#else
		#define GEAR_API __declspec(dllimport)
	#endif
#else
	#error 5th Gear only supports Windows!!
#endif