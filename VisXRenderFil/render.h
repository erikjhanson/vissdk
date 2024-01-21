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

class CRenderInputPin;
class CRender;
class CRenderFilter;


// Main filter object

class CRenderFilter : public CBaseFilter
{
    CRender * const m_pRender;

public:

    // Constructor
    CRenderFilter(CRender *pRender,
                LPUNKNOWN pUnk,
                CCritSec *pLock,
                HRESULT *phr);

    // Pin enumeration
    CBasePin * GetPin(int n);
    int GetPinCount();

};


//  Pin object

class CRenderInputPin : public CRenderedInputPin
{
    CRender    * const m_pRender;           // Main renderer object
    CCritSec * const m_pReceiveLock;        // Critical section

public:

    CRenderInputPin(CRender *pRender,
                  LPUNKNOWN pUnk,
                  CBaseFilter *pFilter,
                  CCritSec *pLock,
                  CCritSec *pReceiveLock,
                  HRESULT *phr);

    // Do something with this media sample
    STDMETHODIMP Receive(IMediaSample *pSample);
    STDMETHODIMP ReceiveCanBlock();

    // Check if the pin can support this specific proposed type and format
    HRESULT CheckMediaType(const CMediaType *);

};


//  CRender object which has filter and pin members

class CRender : public CUnknown,
				public ITransfer
{
    friend class CRenderFilter;
    friend class CRenderInputPin;

    CRenderFilter *m_pFilter;         // Methods for filter interfaces
    CRenderInputPin *m_pPin;          // A simple rendered input pin
    CCritSec m_Lock;                // Main renderer critical section
    CCritSec m_ReceiveLock;         // Sublock for received samples
	void * m_pfnCallback;
	void * m_cbParam;
public:

    DECLARE_IUNKNOWN

    CRender(LPUNKNOWN pUnk, HRESULT *phr);
    ~CRender();

    // ITransfer methods
    STDMETHODIMP SetFrameCallback(void * cbParam, void * pfnCallback);
    
	static CUnknown * WINAPI CreateInstance(LPUNKNOWN punk, HRESULT *phr);

private:

    // Overriden to say what interfaces we support where
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

};

