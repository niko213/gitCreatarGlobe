// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#if GLB_USE_VLD > 0
#   ifndef VLD_FORCE_ENABLE
#   define VLD_FORCE_ENABLE
#   endif 

#include "vld.h"
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
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

