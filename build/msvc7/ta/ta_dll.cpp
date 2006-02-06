// ta.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "ta_def.h"
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

// This is an example of an exported variable
TA_API int nta=0;

// This is an example of an exported function.
TA_API int fnta(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see ta.h for the class definition
Cta::Cta()
{ 
	return; 
}
