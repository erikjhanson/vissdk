//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992 - 2000  Microsoft Corporation.  All Rights Reserved.
//
//--------------------------------------------------------------------------;

//
// A renderer that passes the captured image buffers to the application.
//
//
//
// Files
//
// Render.cpp             Main implementation of the Renderer filter
// Render.def             What APIs the DLL will import and export
// Render.h               Class definition of the derived renderer
// Render.rc              Version information for the sample DLL
// Renduids.h             CLSID for the Render filter
// Itrnsfer.h             ITransfer Interface declaration for callback support
//
// Base classes used
//
// CBaseFilter          Base filter class supporting IMediaFilter
// CRenderedInputPin    An input pin attached to a renderer
// CUnknown             Handle IUnknown for our IFileSinkFilter
// CPosPassThru         Passes seeking interfaces upstream
// CCritSec             Helper class that wraps a critical section
//
//

#include <windows.h>
#include <commdlg.h>

// Need to add <DirectShow Media SDK>\Classes\Base to the include path
// so that we can include Streams.h.
#include <streams.h>

#include <initguid.h>
#include <renduids.h>
#include <itrnsfer.h>
#include "render.h"


//LATER - Set this in sync with the VisDSCamera's callback function.
typedef HRESULT (STDAPICALLTYPE *T_PfnFrameCallback) (void *, void *);

// Setup data

const AMOVIESETUP_MEDIATYPE sudPinTypes =
{
    &MEDIATYPE_NULL,            // Major type
    &MEDIASUBTYPE_NULL          // Minor type
};

const AMOVIESETUP_PIN sudPins =
{
    L"Input",                   // Pin string name
    FALSE,                      // Is it rendered
    FALSE,                      // Is it an output
    FALSE,                      // Allowed none
    FALSE,                      // Likewise many
    &CLSID_NULL,                // Connects to filter
    L"Output",                  // Connects to pin
    1,                          // Number of types
    &sudPinTypes                // Pin information
};

const AMOVIESETUP_FILTER sudRender =
{
#ifdef _DEBUG
    &CLSID_RenderDB,                // Filter CLSID
    L"RenderDB",                    // String name
#else // _DEBUG
    &CLSID_Render,                // Filter CLSID
    L"Render",                    // String name
#endif // _DEBUG
    MERIT_DO_NOT_USE,           // Filter merit
    1,                          // Number pins
    &sudPins                    // Pin details
};


//
//  Object creation stuff
//
CFactoryTemplate g_Templates[]= {
#ifdef _DEBUG
    L"RenderDB", &CLSID_RenderDB, CRender::CreateInstance, NULL, &sudRender
#else // _DEBUG
    L"Render", &CLSID_Render, CRender::CreateInstance, NULL, &sudRender
#endif // _DEBUG
};
int g_cTemplates = 1;


// Constructor

CRenderFilter::CRenderFilter(CRender *pRender,
                         LPUNKNOWN pUnk,
                         CCritSec *pLock,
                         HRESULT *phr) :
#ifdef _DEBUG
    CBaseFilter(NAME("CRenderFilter"), pUnk, pLock, CLSID_RenderDB),
#else // _DEBUG
    CBaseFilter(NAME("CRenderFilter"), pUnk, pLock, CLSID_Render),
#endif // _DEBUG
    m_pRender(pRender)
{
}


//
// GetPin
//
CBasePin * CRenderFilter::GetPin(int n)
{
    if (n == 0) {
        return m_pRender->m_pPin;
    } else {
        return NULL;
    }
}


//
// GetPinCount
//
int CRenderFilter::GetPinCount()
{
    return 1;
}


//
//  Definition of CRenderInputPin
//
CRenderInputPin::CRenderInputPin(CRender *pRender,
                             LPUNKNOWN pUnk,
                             CBaseFilter *pFilter,
                             CCritSec *pLock,
                             CCritSec *pReceiveLock,
                             HRESULT *phr) :

    CRenderedInputPin(NAME("CRenderInputPin"),
                  pFilter,                   // Filter
                  pLock,                     // Locking
                  phr,                       // Return code
                  L"Input"),                 // Pin name
    m_pReceiveLock(pReceiveLock),
    m_pRender(pRender)
{
}


//
// CheckMediaType
//
// Check if the pin can support this specific proposed type and format
//
HRESULT CRenderInputPin::CheckMediaType(const CMediaType *)
{
    return S_OK;
}


//
// ReceiveCanBlock
//
// We don't hold up source threads on Receive
//
STDMETHODIMP CRenderInputPin::ReceiveCanBlock()
{
    return S_FALSE;
}


//
// Receive
//
// Do something with this media sample
//
STDMETHODIMP CRenderInputPin::Receive(IMediaSample *pSample)
{
    CAutoLock lock(m_pReceiveLock);

    // Call the app's callback with the pointer to the Media Sample

    return ((T_PfnFrameCallback)m_pRender->m_pfnCallback)(m_pRender->m_cbParam, pSample);
}


//
//  CRender class
//
CRender::CRender(LPUNKNOWN pUnk, HRESULT *phr) :
    CUnknown(NAME("CRender"), pUnk),
    m_pFilter(NULL),
    m_pPin(NULL),
    m_pfnCallback(0),
    m_cbParam(0)
{
    m_pFilter = new CRenderFilter(this, GetOwner(), &m_Lock, phr);
    if (m_pFilter == NULL) {
        *phr = E_OUTOFMEMORY;
        return;
    }

    m_pPin = new CRenderInputPin(this,GetOwner(),
                               m_pFilter,
                               &m_Lock,
                               &m_ReceiveLock,
                               phr);
    if (m_pPin == NULL) {
        *phr = E_OUTOFMEMORY;
        return;
    }
}

HRESULT CRender::SetFrameCallback(void * cbParam, void * pfnCallback)
{
    CAutoLock lock(&m_Lock);
    m_pfnCallback = pfnCallback;
    m_cbParam = cbParam;
    return NOERROR;
}


// Destructor

CRender::~CRender()
{
    delete m_pPin;
    delete m_pFilter;
}


//
// CreateInstance
//
// Provide the way for COM to create a Render filter
//
CUnknown * WINAPI CRender::CreateInstance(LPUNKNOWN punk, HRESULT *phr)
{
    CRender *pNewObject = new CRender(punk, phr);
    if (pNewObject == NULL) {
        *phr = E_OUTOFMEMORY;
    }
    return pNewObject;

} // CreateInstance


//
// NonDelegatingQueryInterface
//
// Override this to say what interfaces we support where
//
STDMETHODIMP CRender::NonDelegatingQueryInterface(REFIID riid, void ** ppv)
{
    CheckPointer(ppv,E_POINTER);
    CAutoLock lock(&m_Lock);

    // Do we have this interface

    if (riid == IID_ITransfer) {
        return GetInterface((ITransfer *) this, ppv);
    } else if (riid == IID_IBaseFilter || riid == IID_IMediaFilter || riid == IID_IPersist) {
	return m_pFilter->NonDelegatingQueryInterface(riid, ppv);
    }  else {
	return CUnknown::NonDelegatingQueryInterface(riid, ppv);
    }

} // NonDelegatingQueryInterface



//
// DllRegisterSever
//
// Handle the registration of this filter
//
STDAPI DllRegisterServer()
{
    return AMovieDllRegisterServer2( TRUE );

} // DllRegisterServer


//
// DllUnregisterServer
//
STDAPI DllUnregisterServer()
{
    return AMovieDllRegisterServer2( FALSE );

} // DllUnregisterServer
