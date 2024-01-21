// VisMeteorDLL.h
//
// Copyright © 1997-2000 Microsoft Corporation, All Rights Reserved


extern HANDLE g_hModule;

// The prefix for cameras handled by this DLL.
extern const char *g_szMeteorProviderID;

extern const char *g_szMeteorProviderName;



EXTERN_C
{
	_declspec(dllexport) HRESULT STDAPICALLTYPE VisGetImSrcProvider
			(LPUNKNOWN pUnkOuter, const char *szProvider, REFIID riid,
			void **ppvObject);
};



// DLL entry point (called from standard library).  Used to find the
// handle for this DLL.
BOOL APIENTRY DllMain(HANDLE hModule, 
                      DWORD  ulReasonForCall, 
                      LPVOID lpReserved);
