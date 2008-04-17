#ifndef AUDIOPROC_DEF_H
#define AUDIOPROC_DEF_H

#undef QT_SHARED                             // Already defined in config.h.
#include "ta_global.h" // needed everywhere

#if (defined(AUDIOPROC_DLL) && (defined(TA_OS_WIN) && defined(_MSC_VER)) && (!defined(__MAKETA__)))
  #ifdef AUDIOPROC_EXPORTS
  #define AUDIOPROC_API __declspec(dllexport)
  #else
  #define AUDIOPROC_API __declspec(dllimport)
  #endif
#else 
# define AUDIOPROC_API    // Needed for windows compatability.
#endif


#include "audioproc_TA_type.h" // types for this plugin

#endif
