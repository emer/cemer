// pdp.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "pdp_def.h"
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
/*
// This is an example of an exported variable
PDP_API int npdp=0;

// This is an example of an exported function.
PDP_API int fnpdp(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see pdp.h for the class definition
Cpdp::Cpdp()
{ 
	return; 
}
*/