// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the TAIQTSO_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// TAIQTSO_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef TAIQTSO_EXPORTS
#define TAIQTSO_API __declspec(dllexport)
#else
#define TAIQTSO_API __declspec(dllimport)
#endif

// This class is exported from the taiqtso.dll
class TAIQTSO_API Ctaiqtso {
public:
	Ctaiqtso(void);
	// TODO: add your methods here.
};

extern TAIQTSO_API int ntaiqtso;

TAIQTSO_API int fntaiqtso(void);
