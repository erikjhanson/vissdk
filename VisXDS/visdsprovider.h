////////////////////////////////////////////////////////////////////////////
//
// @doc INTERNAL EXTERNAL VISDSPROVIDER
//
// @module VisDSProvider.h |
//
// This file defines the <c CVisDSProvider> class.  It is derived from
// the <c IVisImSrcProvider> interface.  It is used to get a list of
// available DS devices and to create a <c CVisDSCamera> object for
// a specified device.
//
// <nl>
//
// Copyright © 1997-2000 Microsoft Corporation, All Rights Reserved
// <nl>
//
// @xref <l VisDSProvider\.cpp>
//
////////////////////////////////////////////////////////////////////////////


#ifndef __VIS_IMSRC_VISDSPROVIDER_H__
#define __VIS_IMSRC_VISDSPROVIDER_H__


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000



class CVisDSProvider : public IVisImSrcProvider
{
public:
	//------------------------------------------------------------------
	// Constructor and destructor

    CVisDSProvider(void);
	virtual ~CVisDSProvider(void);


	//------------------------------------------------------------------
	// IUnknown methods

    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppvObject);
    STDMETHOD_(ULONG, AddRef) (THIS);
    STDMETHOD_(ULONG, Release) (THIS);


	//------------------------------------------------------------------
	// IVisImSrcProvider methods

    STDMETHOD(HasConnectedDevices) (THIS);
    STDMETHOD(HasSelectionDialog) (THIS);
    STDMETHOD_(LONG, DoSelectionDialog) (THIS_
			HWND hwndParent VISDEFAULT(0));
    STDMETHOD(GetConnectedDeviceList) (THIS_ IVisListSz **ppIVisListSz);
    STDMETHOD(GetDeviceSz) (THIS_ IVisImSrcDevice **ppIVisImSrcDevice,
			LPCSTR sz);
    STDMETHOD(GetDeviceI) (THIS_ IVisImSrcDevice **ppIVisImSrcDevice,
			LONG i);
    STDMETHOD(GetSzID) (THIS_ LPSTR sz, LONG *pcb);
    STDMETHOD(GetSzName) (THIS_ LPSTR sz, LONG *pcb);


private:
	enum { e_iDSIDLim = 10 };

	// LATER:  It might be okay to keep the provider ID, but we should
	// probably get the name from the registry.
	static const char *s_szID;		// LATER:  Replace global string
	static const char *s_szName;	// LATER:  Replace global string

	unsigned long m_cRef;
	CVisListSz *m_plistszConnectedDevices;
	IEnumMoniker *m_pEMDevices;

	void  FindConnectedDevices(void);
};


#endif // __VIS_IMSRC_VISDSPROVIDER_H__
