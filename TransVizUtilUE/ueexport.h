#pragma once

#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
#   if defined(TRANSVIZ_UTIL_UE_LIBRARY)
#      define TRANSVIZUE_UTIL_DLL_EXPORT __declspec(dllexport)
#   else
#      define TRANSVIZUE_UTIL_DLL_EXPORT __declspec(dllimport)
#   endif 
#else
#   if defined(TRANSVIZ_UTIL_UE_LIBRARY)
#      define TRANSVIZUE_UTIL_DLL_EXPORT __attribute__ ((visibility("default")))
#   else
#      define TRANSVIZUE_UTIL_DLL_EXPORT
#   endif 
#endif
