// VisMeteorDLL.cpp
//
// Copyright © 1997-2000 Microsoft Corporation, All Rights Reserved


// Define INITGUID here to get GUIDs defined in the header files.
// (We'll also need to link with uuid.lib to get some standard
// GUIDs, like IID_IUnknown.)
#define INITGUID

#include "VisMeteorPch.h"
#include "VisMeteor.h"


// Global variables.

// The handle of this DLL.
HANDLE g_hModule = 0;

// The prefix for cameras handled by this DLL.
extern const char *g_szMeteorProviderID = "VisMeteor";

#ifdef _DEBUG
const char *g_szMeteorProviderName = "Matrox Meteor Card";
#else // _DEBUG
const char *g_szMeteorProviderName = "Matrox Meteor Card (Debug)";
#endif // _DEBUG



BOOL APIENTRY DllMain(HANDLE hModule, 
                      DWORD  ulReasonForCall, 
                      LPVOID lpReserved)
{
    switch(ulReasonForCall)
	{
		case DLL_PROCESS_ATTACH:
			if (hModule != 0)
			{
				g_hModule = hModule;
				DisableThreadLibraryCalls((HMODULE)hModule);
			}
		break;

		case DLL_THREAD_ATTACH:
			if (g_hModule == 0)
				g_hModule = hModule;
		break;

		case DLL_THREAD_DETACH:
		break;

		case DLL_PROCESS_DETACH:
		break;
    }

    return TRUE;
}


EXTERN_C
{
	_declspec(dllexport) HRESULT STDAPICALLTYPE VisGetImSrcProvider
			(LPUNKNOWN pUnkOuter, const char *szProvider, REFIID riid,
			void **ppvObject)
	{
		HRESULT hrRet;

		if (ppvObject == 0)
		{
			hrRet = ResultFromScode(E_POINTER);
		}
		else if (pUnkOuter != 0)
		{
			hrRet = ResultFromScode(CLASS_E_NOAGGREGATION);
			*ppvObject = 0;
		}
		else if (szProvider == 0)
		{
			hrRet = ResultFromScode(E_POINTER);
			*ppvObject = 0;
		}
		else if ((lstrcmpi(szProvider, g_szMeteorProviderID) != 0)
				&& (lstrcmpi(szProvider, g_szMeteorProviderName) != 0))
		{
			hrRet = ResultFromScode(E_FAIL);
			*ppvObject = 0;
		}
		else if ((riid == IID_IUnknown)
				|| (riid == IID_IVisImSrcProvider))
		{
			*ppvObject = (IVisImSrcProvider *) CVisMeteorProvider::PMeteorProviderGet();
			if (*ppvObject == 0)
				hrRet = ResultFromScode(E_OUTOFMEMORY);
			else
				hrRet = ResultFromScode(S_OK);
		}
		else
		{
			*ppvObject = 0;
			hrRet = ResultFromScode(E_NOINTERFACE);
		}

		return hrRet;
	}
};
