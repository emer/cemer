// taiqtso.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "taiqtso.h"
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
TAIQTSO_API int ntaiqtso=0;

// This is an example of an exported function.
TAIQTSO_API int fntaiqtso(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see taiqtso.h for the class definition
Ctaiqtso::Ctaiqtso()
{ 
	return; 
}
