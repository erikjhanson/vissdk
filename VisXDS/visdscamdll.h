////////////////////////////////////////////////////////////////////////////
//
// @doc INTERNAL EXTERNAL VISDSCAMDLL
//
// @module VisDSCamDLL.h |
//
// This file defines the exported function <f VisGetImSrcProvider>, which
// is used to create a <c CVisDSProvider> object.  It also defines
// the DLL entry point, DllMain, and two global string variables.
//
// <nl>
//
// Copyright © 1997-2000 Microsoft Corporation, All Rights Reserved
// <nl>
//
// @xref <l VisDSCamDLL\.cpp>
//
////////////////////////////////////////////////////////////////////////////


#ifndef __VIS_IMSRC_VISDSCAMDLL_H__
#define __VIS_IMSRC_VISDSCAMDLL_H__


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


extern HANDLE g_hModule;

// The prefix for cameras handled by this DLL.
extern const char *g_szDSProviderID;

extern const char *g_szDSProviderName;



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


#endif // __VIS_IMSRC_VISDSCAMDLL_H__
