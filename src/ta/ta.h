// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the TA_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// TA_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef TA_EXPORTS
#define TA_API __declspec(dllexport)
#else
#define TA_API __declspec(dllimport)
#endif

// This class is exported from the ta.dll
class TA_API Cta {
public:
	Cta(void);
	// TODO: add your methods here.
};

extern TA_API int nta;

TA_API int fnta(void);
