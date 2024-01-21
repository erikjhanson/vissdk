////////////////////////////////////////////////////////////////////////////
//
// @doc INTERNAL EXTERNAL VISDSPROVIDER 
//
// @module VisDSProvider.cpp |
//
// This file implements the <c CVisDSProvider> class.  It is derived from
// the <c IVisImSrcProvider> interface.  It is used to get a list of
// available Direct Show devices and to create a <c CVisDSCamera> object for
// a specified device.
//
// <nl>
//
// Copyright © 1997-2000 Microsoft Corporation, All Rights Reserved
// <nl>
//
// @xref <l VisDSProvider\.h>
//
////////////////////////////////////////////////////////////////////////////


#include "VisDSCamera.h"


const char *CVisDSProvider::s_szID = "VisXDS";

#ifdef _DEBUG
const char *CVisDSProvider::s_szName = "Direct Show";
#else // _DEBUG
const char *CVisDSProvider::s_szName = "Direct Show (Debug)";
#endif // _DEBUG



CVisDSProvider::CVisDSProvider(void)
  : m_cRef(1),
	m_plistszConnectedDevices(0),
	m_pEMDevices(0)
{
}

CVisDSProvider::~CVisDSProvider(void)
{
	if (m_plistszConnectedDevices != 0)
	{
		m_plistszConnectedDevices->Release();
		m_plistszConnectedDevices = 0;
	}

	if (m_pEMDevices)
		m_pEMDevices->Release();
}



STDMETHODIMP CVisDSProvider::QueryInterface(THIS_ REFIID riid,
		void **ppvObject)
{
	HRESULT hrRet;

	if (ppvObject == 0)
	{
		hrRet = ResultFromScode(E_POINTER);
	}
	else if ((riid == IID_IUnknown)
				|| (riid == IID_IVisImSrcProvider))
	{
		hrRet = ResultFromScode(S_OK);
		*ppvObject = (IVisImSrcProvider *) this;
		AddRef();
	}
	else
	{
		*ppvObject = 0;
		hrRet = ResultFromScode(E_NOINTERFACE);
	}

	return hrRet;
}

STDMETHODIMP_(ULONG) CVisDSProvider::AddRef(THIS)
{
	assert(m_cRef > 0);

	return ++m_cRef;
}

STDMETHODIMP_(ULONG) CVisDSProvider::Release(THIS)
{
	unsigned long cRef = --m_cRef;

	if (cRef == 0)
		delete this;

	return cRef;
}



STDMETHODIMP CVisDSProvider::HasConnectedDevices(THIS)
{
	if (m_plistszConnectedDevices == 0)
		FindConnectedDevices();
	
	HRESULT hrRet;

	if (m_plistszConnectedDevices == 0)
		hrRet = ResultFromScode(E_FAIL);
	else if (m_plistszConnectedDevices->CszGet() == 0)
		hrRet = ResultFromScode(S_FALSE);
	else
		hrRet = ResultFromScode(S_OK);

	return hrRet;
}

STDMETHODIMP CVisDSProvider::HasSelectionDialog(THIS)
{
	return ResultFromScode(S_FALSE);
}

STDMETHODIMP_(LONG) CVisDSProvider::DoSelectionDialog(THIS_
		HWND hwndParent)
{
	return - 1;
}

STDMETHODIMP CVisDSProvider::GetConnectedDeviceList(THIS_
		IVisListSz **ppIVisListSz)
{
	if (m_plistszConnectedDevices == 0)
		FindConnectedDevices();
	
	HRESULT hrRet;

	if (ppIVisListSz == 0)
	{
		hrRet = ResultFromScode(E_POINTER);
	}
	else
	{
		if (m_plistszConnectedDevices == 0)
			hrRet = ResultFromScode(E_FAIL);
		else
			hrRet = ResultFromScode(S_OK);

		*ppIVisListSz = m_plistszConnectedDevices;

		if (m_plistszConnectedDevices != 0)
			m_plistszConnectedDevices->AddRef();
	}

	return hrRet;
}

STDMETHODIMP CVisDSProvider::GetDeviceSz(THIS_
		IVisImSrcDevice **ppIVisImSrcDevice, LPCSTR sz)
{
	// UNDONE:  We shouldn't need to find the list of all devices here.  We
	// only need to attempt to instantiate devices that have the name specified.
	if (m_plistszConnectedDevices == 0)
		FindConnectedDevices();
	
	HRESULT hrRet;

	if (ppIVisImSrcDevice == 0)
	{
		hrRet = ResultFromScode(E_POINTER);
	}
	else if (sz == 0)
	{
		hrRet = ResultFromScode(E_INVALIDARG);
		*ppIVisImSrcDevice = 0;
	}
	else if (m_plistszConnectedDevices == 0)
	{
		hrRet = ResultFromScode(E_FAIL);
		*ppIVisImSrcDevice = 0;
	}
	else
	{
		hrRet = GetDeviceI(ppIVisImSrcDevice,
				m_plistszConnectedDevices->IGetSz(sz));
	}

	return hrRet;
}

STDMETHODIMP CVisDSProvider::GetDeviceI(THIS_
		IVisImSrcDevice **ppIVisImSrcDevice, LONG i)
{
	// UNDONE:  We shouldn't need to find the list of all devices here.  We
	// only need to attempt to instantiate i devices to find the device specified.
	if (m_plistszConnectedDevices == 0)
		FindConnectedDevices();
	
	HRESULT hrRet;

	if (ppIVisImSrcDevice == 0)
	{
		hrRet = ResultFromScode(E_POINTER);
	}
	else if (m_plistszConnectedDevices == 0)
	{
		hrRet = ResultFromScode(E_FAIL);
		*ppIVisImSrcDevice = 0;
	}
	else if ((i < 0) || (i >= m_plistszConnectedDevices->CszGet()))
	{
		hrRet = ResultFromScode(E_INVALIDARG);
		*ppIVisImSrcDevice = 0;
	}
	else
	{
		CVisDSCamera *pCamera = new CVisDSCamera();
		
		if (pCamera == 0)
			hrRet = ResultFromScode(E_OUTOFMEMORY);
		else
			hrRet = pCamera->Init(m_pEMDevices, i);

		if (FAILED(hrRet))
		{
			delete pCamera;
			pCamera = 0;
		}

		*ppIVisImSrcDevice = pCamera;
	}

	return hrRet;
}

STDMETHODIMP CVisDSProvider::GetSzID(THIS_ LPSTR sz, LONG *pcb)
{
	return VisGetSzPcbFromSz(sz, pcb, s_szID);
}

STDMETHODIMP CVisDSProvider::GetSzName(THIS_ LPSTR sz, LONG *pcb)
{
	return VisGetSzPcbFromSz(sz, pcb, s_szName);
}


void  CVisDSProvider::FindConnectedDevices(void)
{
	HRESULT hr;
	if (m_plistszConnectedDevices == 0)
	{
		CVisListSz *plistsz = new CVisListSz;

	    // enumerate all video capture devices
		ICreateDevEnum *pCreateDevEnum;
		hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
			                  IID_ICreateDevEnum, (void**)&pCreateDevEnum);
		if (hr != NOERROR)
			goto Error;
		hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,
			                                                        &m_pEMDevices, 0);
		pCreateDevEnum->Release();
		if (hr != NOERROR) {
			fprintf(stderr, "Sorry, you have no video capture hardware");
			goto Error;
		}
		m_pEMDevices->Reset();
		ULONG cFetched;
		IMoniker *pM;
		while(hr = m_pEMDevices->Next(1, &pM, &cFetched), hr==S_OK)
		{
			IPropertyBag *pBag;
			hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
			if(SUCCEEDED(hr)) {
				VARIANT var;
				var.vt = VT_BSTR;
				hr = pBag->Read(L"FriendlyName", &var, NULL);
				if (hr == NOERROR) {
					char achName[80];
					WideCharToMultiByte(CP_ACP, 0, var.bstrVal, -1, achName, 80,
						                                            NULL, NULL);
					plistsz->AddSz(achName);

					SysFreeString(var.bstrVal);
				}
				pBag->Release();
			}
			pM->Release();
		}

Error:
		m_plistszConnectedDevices = plistsz;
	}
	return;
}
