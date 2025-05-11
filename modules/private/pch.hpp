#pragma once

#ifdef AUXILIARY_DLL
#	ifdef _WIN32
#		define AUXILIARY_API __declspec(dllexport)
#	endif
#endif