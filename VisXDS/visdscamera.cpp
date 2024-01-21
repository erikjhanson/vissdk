////////////////////////////////////////////////////////////////////////////
//
// @doc INTERNAL EXTERNAL VISDSCAMERA 
//
// @module VisDSCamera.cpp |
//
// This file implements the <c CVisDSCamera> class.  It is derived from
// the <c IVisImSrcSettings> and <c IVisImSrcDevice> interfaces.
// It is used to work with (adjust settings for and get images from)
// a DS camera.
//
// <nl>
//
// Copyright © 1997-2000 Microsoft Corporation, All Rights Reserved
// <nl>
//
// @xref <l VisDSCamera\.h>
//
////////////////////////////////////////////////////////////////////////////



#include <process.h>

#include "VisDSCamera.h"


const bool gvis_fPreferDirectShowDialogsToVFWDialogs = true;


CVisDSCamera::CVisDSCamera(void)
  : m_cRef(1),
	m_strID(),
	pBuilder(0),
	pFg(0),
	pVCap(0),
	pVTrns(0),
	pTransfer(0),
	pVSC(0),
	pCC(0),
	pVPA(0),
	pMC(0),
	pDlg(0),
	m_evisimencoding(evisimencodingUnknown),
	m_evisconvyuv(evisconvyuvDefault),
	m_evisimencodingGrabCur(evisimencodingUnknown),
	m_pfnCallback(0),
	m_pvUser(0),
	m_cdwFourCCUnknownCur(0),
	m_fInContinuousGrab(false),
	m_fInSingleFrameGrab(false),
	m_fFlippedImage(true),
	m_fUseNegHtToFlip(false),
	m_fCanSetVideoFormat(true),
	m_ImageWidth(0),
	m_ImageHeight(0),
	m_SingleFrameSemaphore(0)
{

	int i = 0;

	for (i = 0; i < evisimencodingLim; ++i)
	{
		m_rgdwFourCCImEncoding[i] = 0;
		m_rgeflipImEncoding[i] = eflipUnknown;
		m_rgfHaveImEncoding[i] = false;
	}
	
	return;
}

CVisDSCamera::~CVisDSCamera(void)
{
	//Stop the graph first. LATER - may want to add debug messages here.
	if (pMC) {
		pMC->Stop();
		pMC->Release();
	}

	if (pBuilder && pFg && pVTrns && pVCap) {
		DisconnectFilters();
	}

	if (pFg) {
		pFg->RemoveFilter(pVTrns);
		pFg->RemoveFilter(pVCap);
        pFg->Release();
	}


    if (pBuilder)
        pBuilder->Release();

    if (pVCap)
        pVCap->Release();

    if (pVSC)
        pVSC->Release();

	if (pCC)
        pCC->Release();

	if (pVPA)
        pVPA->Release();

	if (pDlg)
        pDlg->Release();

	if (pVTrns)
		pVTrns->Release();
	
	if (pTransfer)
		pTransfer->Release();

	if (m_SingleFrameSemaphore)
		CloseHandle(m_SingleFrameSemaphore);

}

STDMETHODIMP CVisDSCamera::QueryInterface(THIS_ REFIID riid, void **ppvObject)
{
	HRESULT hrRet;

	if (ppvObject == 0)
	{
		hrRet = ResultFromScode(E_POINTER);
	}
	else if ((riid == IID_IUnknown)
				|| (riid == IID_IVisImSrcSettings))
	{
		hrRet = ResultFromScode(S_OK);
		*ppvObject = (IVisImSrcSettings *) this;
		AddRef();
	}
	else if (riid == IID_IVisImSrcDevice)
	{
		hrRet = ResultFromScode(S_OK);
		*ppvObject = (IVisImSrcDevice *) this;
		AddRef();
	}
	else if (riid == IID_IVisImSrcDevice2)
	{
		hrRet = ResultFromScode(S_OK);
		*ppvObject = (IVisImSrcDevice2 *) this;
		AddRef();
	}
	else
	{
		*ppvObject = 0;
		hrRet = ResultFromScode(E_NOINTERFACE);
	}

	return hrRet;
}

STDMETHODIMP_(ULONG) CVisDSCamera::AddRef(THIS)
{
	assert(m_cRef > 0);

	return ++m_cRef;
}

STDMETHODIMP_(ULONG) CVisDSCamera::Release(THIS)
{
	unsigned long cRef = --m_cRef;

	if (cRef == 0)
		delete this;

	return cRef;
}

// Returns S_OK if successful.
HRESULT CVisDSCamera::Init(IEnumMoniker *pEm, int iCaptureDevice)
{
    BOOL f;
    UINT uIndex = 0;

	HRESULT hr;

	m_strID = g_szDSProviderID;
	m_strID += ":  ";
	assert(ecchProviderSep == 3);

	//Create the single frame semaphore.
	m_SingleFrameSemaphore = CreateSemaphore(0, 0, 1, 0);
	if (!m_SingleFrameSemaphore) {
		OutputDebugString("Could not create the semaphore for single frame grab");
		return ResultFromScode(E_FAIL);
	}

	//Build the capture graph
    f = MakeBuilder();
    if (!f) {
        hr = ResultFromScode(E_FAIL);
		OutputDebugString("Cannot instantiate graph builder");
		goto Error;
    }

	//
	// First, we need a Video Capture filter, and some interfaces
	//

    // Enumerate all the video devices.  We want iCaptureDevice. 

    pEm->Reset();
    ULONG cFetched;
    IMoniker *pM;
    while(hr = pEm->Next(1, &pM, &cFetched), hr==S_OK)
    {
        // this is the one we want.  Get its name, and instantiate it.
        if ((int)uIndex == iCaptureDevice) {
            IPropertyBag *pBag;
            hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
            if(SUCCEEDED(hr)) {
                VARIANT var;
                var.vt = VT_BSTR;
                hr = pBag->Read(L"FriendlyName", &var, NULL);
                if (hr == NOERROR) {
					char szT[80];
					*szT = 0;
                    WideCharToMultiByte(CP_ACP, 0, var.bstrVal, -1,
                                        szT, 80, NULL, NULL);
					m_strID += szT;
                    SysFreeString(var.bstrVal);
                }
                pBag->Release();
            }
            hr = pM->BindToObject(0, 0, IID_IBaseFilter, (void**)&pVCap);
            pM->Release();
            break;
        }
        pM->Release();
        uIndex++;
    }

    if (pVCap == NULL) {
        OutputDebugString("Error : Cannot create video capture filter");
        goto Error;
    }

	
	//fetch the renderer filter
	#ifdef _DEBUG
		hr = CoCreateInstance(CLSID_RenderDB, NULL, CLSCTX_INPROC_SERVER,
							  IID_IBaseFilter, (void**)&pVTrns);
	#else // _DEBUG
		hr = CoCreateInstance(CLSID_Render, NULL, CLSCTX_INPROC_SERVER,
							  IID_IBaseFilter, (void**)&pVTrns);
	#endif // _DEBUG

	if (hr != NOERROR)
        goto Error;
    

	//initialize the filter interfaces
    //
    // make a filtergraph, give it to the graph builder and put the video
    // capture filter in the graph
    //

    f = MakeGraph();
    if (!f) {
        hr = ResultFromScode(E_FAIL);
        OutputDebugString("Cannot instantiate filtergraph");
        goto Error;
    }
    hr = pBuilder->SetFiltergraph(pFg);
    if (hr != NOERROR) {
        OutputDebugString("Cannot give graph to builder");
        goto Error;
    }

    hr = pFg->AddFilter(pVCap, NULL);
    if (hr != NOERROR) {
        OutputDebugString("Error : Cannot add vidcap to filtergraph");
        goto Error;
    }


	hr = pFg->AddFilter(pVTrns, NULL);
    if (hr != NOERROR) {
        OutputDebugString("Error : Cannot add renderer filter to filtergraph");
        goto Error;
    }

    // connect the capture and render filters.
	hr = ConnectFilters();
    if (hr != NOERROR)
        goto Error;
  
	//fetch the ITransfer interface
	hr = pVTrns->QueryInterface(IID_ITransfer, (void **)&pTransfer);
	if (hr != NOERROR)
        goto Error;
    
    // Calling FindInterface below will result in building the upstream
    // section of the capture graph (any WDM TVTuners or Crossbars we might
    // need).

    // !!! What if this interface isn't supported?
    // we use this interface to set the frame rate and get the capture size
    hr = pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, pVCap,
                                IID_IAMStreamConfig, (void **)&pVSC);
    if (hr != NOERROR) {
        // this means we can't set frame rate (non-DV only)
        OutputDebugString("Error : Cannot find VCapture:IAMStreamConfig");
    }

    hr = pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, pVCap,
                                IID_IAMVideoProcAmp, (void **)&pCC);
    if (hr != NOERROR) {
        OutputDebugString("Error : Cannot find VCapture:IAMCameraControl");
    }

    hr = pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, pVCap,
                                IID_IAMVideoProcAmp, (void **)&pVPA);
    if (hr != NOERROR) {
        OutputDebugString("Error : Cannot find VCapture:IAMVideoProcAmp");
    }

    // we use this interface to bring up the 3 dialogs
    // NOTE:  Only the VfW capture filter supports this.  This app only brings
    // up dialogs for legacy VfW capture drivers, since only those have dialogs
    hr = pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, pVCap,
                                IID_IAMVfwCaptureDialogs, (void **)&pDlg);
	
    // prepare to run the graph
	hr = pFg->QueryInterface(IID_IMediaControl, (void **)&pMC);
	if (FAILED(hr)) {
		OutputDebugString("Error : Cannot get IMediaControl");
	}

	hr = pMC->Run();

	//bugbug: remove voids
	hr = pTransfer->SetFrameCallback((void *)this, (void *)StaticFrameCallbackProc);
    
	if (hr == NOERROR)
	{
		SetupCaptureParams();

		if (!FReadSettings())
		{
			// if we can't read the settings (or determine the type of images
			// that we're getting), fail the initialization.
			hr = ResultFromScode(E_FAIL);
		}
		else
		{
			UpdateCachedInfo();
		}
	}

Error:
	return hr;
}


// Get a string identifying this device.
// Returns the number of bytes copied in pcb if successful.
STDMETHODIMP CVisDSCamera::GetSzID(THIS_ char *sz, LONG *pcb)
{
	return VisGetSzPcbFromSz(sz, pcb, m_strID.c_str());
}

const char *CVisDSCamera::SzName(void) const
{
	int cchPrefix = strlen(g_szDSProviderID) + ecchProviderSep;
	assert(*(m_strID.c_str() + strlen(g_szDSProviderID)) == ':');
	assert(*(m_strID.c_str() + cchPrefix - 1) == ' ');
	assert(*(m_strID.c_str() + cchPrefix) != ' ');

	return m_strID.c_str() + cchPrefix;
}

STDMETHODIMP CVisDSCamera::GetSzName(THIS_ char *sz, LONG *pcb)
{
	return VisGetSzPcbFromSz(sz, pcb, SzName());
}

STDMETHODIMP CVisDSCamera::SaveSettings(THIS_ bool fForAllApps)
{
	HRESULT hrRet = ResultFromScode(E_FAIL);

	// Find the registry key.
	std::string strKey = VIS_SZ_REG_IMSRC_DEVS  "\\";
	strKey += g_szDSProviderID;
	strKey += "\\";
	strKey += SzName();

#ifdef LATER
	// This code is not needed now, but could be used to get an App ID
	// to use in app-specific keys.
	char szFullAppName[MAX_PATH];
	const char *szAppID = 0;
	if (GetModuleFileName(0, szFullAppName, sizeof(szFullAppName)) != 0)
	{
		szAppID = strrchr(szFullAppName, '\\');
		if (szAppID != 0)
			++ szAppID;
		else
			szAppID = szFullAppName;
	}
#endif // LATER

	// Attempt to open the reg key.
	HKEY hkey;
	if (VisFCreateRegKey(&hkey, strKey.c_str()))
	{
		std::string strVal;

		// First, write information about any unknown dwFourCC values that
		// we found.  (Write this even if we don't write any other setting
		// information.)
		for (int iUnknown = 0; iUnknown < m_cdwFourCCUnknownCur; ++ iUnknown)
		{
			char szNumUnknown[4];
			assert(iUnknown < 10);
			szNumUnknown[0] = '0' + iUnknown;
			szNumUnknown[1] = 0;

			strVal = "UnknownFourCC";
			strVal += szNumUnknown;
			VisFWriteDwordToReg(m_rgdwFourCCUnknown[iUnknown], hkey,
					strVal.c_str());

			strVal = "UnknownCbit";
			strVal += szNumUnknown;
			VisFWriteIntToReg(m_rgcbitFourCCUnknown[iUnknown], hkey,
					strVal.c_str());
		}

		bool fFoundImEncoding = false;
		for (int i = evisimencodingFirst; i < evisimencodingLim; ++ i)
		{
			if ((i == evisimencodingUnknown)
					|| (!m_rgfHaveImEncoding[i]))
			{
				continue;
			}

			assert(VisSzGetImEncoding((EVisImEncoding) i) != 0);

			fFoundImEncoding = true;

			strVal = "EVisImEncoding";
			strVal += VisSzGetImEncoding((EVisImEncoding) i);
			VisFWriteDwordToReg(m_rgdwFourCCImEncoding[i], hkey,
					strVal.c_str());

			assert((m_rgeflipImEncoding[i] == eflipUnknown)
					|| (m_rgeflipImEncoding[i] == eflipNo)
					|| (m_rgeflipImEncoding[i] == eflipYes));

			if (m_rgeflipImEncoding[i] != eflipUnknown)
			{
				strVal = "Flipped";
				strVal += VisSzGetImEncoding((EVisImEncoding) i);
				bool f = ((m_rgeflipImEncoding[i] == eflipNo) ? false : true);
				VisFWriteBoolToReg(f, hkey, strVal.c_str());
			}
		}

		if (!fFoundImEncoding)
		{
			// The camera was not properly initialized, so no settings
			// will be written.
			hrRet = ResultFromScode(S_FALSE);
		}
		else
		{
			hrRet = ResultFromScode(S_OK);
			VisFWriteDwordToReg(m_ImageWidth, hkey, "Width");
			VisFWriteDwordToReg(m_ImageHeight, hkey, "Height");

			VisFWriteBoolToReg(m_fFlippedImage, hkey, "Flipped");
			VisFWriteBoolToReg(m_fUseNegHtToFlip, hkey, "UseNegHeightToFlip");
			VisFWriteBoolToReg(m_fCanSetVideoFormat, hkey, "CanSetVideoFormat");

			VisFWriteIntToReg(m_evisconvyuv, hkey, "EVisConvYUV");

			assert(m_evisimencoding != evisimencodingUnknown);
			VisFWriteIntToReg(m_evisimencoding, hkey, "EVisImEncoding");
		}

		VisFCloseRegKey(hkey);
	}

	return hrRet;
}


// Properties of this image source.
STDMETHODIMP CVisDSCamera::GetImSrcRange(THIS_ EVisImSrc evisimsrc, LONG *plMin,
		LONG *plMax, LONG *pdlStepping, LONG *plDefault, LONG *plFlags) CONST
{
	HRESULT hrRet = ResultFromScode(S_OK);

	LONG lMin = 0;
	LONG lMax = 0;
	LONG dlStepping = 1;
	LONG lDefault = 0;
	LONG lFlags = 0;

	switch (evisimsrc)
	{
	case evisimsrcFContinuousGrab:
		lMax = 1;
		dlStepping = 1;
		lDefault = 0;
		lFlags = evisimsrcflagAuto;
		hrRet = ResultFromScode(S_OK);
		break;

	case evisimsrcFReflectVert:
		lMax = 1;
		dlStepping = 1;
		lDefault = 1;
		lFlags = evisimsrcflagAuto;
		hrRet = ResultFromScode(S_OK);
		break;

	case evisimsrcWidth:
		// Assume that VGA is the best that we can do.
		lMax = 640;
		lFlags = evisimsrcflagAuto;
		break;

	case evisimsrcHeight:
		// Assume that VGA is the best that we can do.
		lMax = 480;
		lFlags = evisimsrcflagAuto;
		break;

	case evisimsrcImEncoding:
		lMax = evisimencodingLim - 1;
		lFlags = evisimsrcflagAuto;
		break;

	case evisimsrcNBuffers:
		//CHECK: 1) Why do we need this. 2)Does CAPSTATUS!wNumAllocated returns what we want?
		lMin = 1;
		lMax = 1;
		break;

	case evisimsrcBrightness:
		hrRet = ResultFromScode(E_FAIL);

		if (pVPA)
		{
			hrRet = pVPA->GetRange(VideoProcAmp_Brightness, &lMin, &lMax, &dlStepping, &lDefault, &lFlags);
		}

		break;

	case evisimsrcContrast:
		hrRet = ResultFromScode(E_FAIL);

		if (pVPA)
		{
			hrRet = pVPA->GetRange(VideoProcAmp_Contrast, &lMin, &lMax, &dlStepping, &lDefault, &lFlags);
		}

		break;

	case evisimsrcHue:
		hrRet = ResultFromScode(E_FAIL);

		if (pVPA)
		{
			hrRet = pVPA->GetRange(VideoProcAmp_Hue, &lMin, &lMax, &dlStepping, &lDefault, &lFlags);
		}

		break;

	case evisimsrcSaturation:
		hrRet = ResultFromScode(E_FAIL);

		if (pVPA)
		{
			hrRet = pVPA->GetRange(VideoProcAmp_Saturation, &lMin, &lMax, &dlStepping, &lDefault, &lFlags);
		}

		break;

	case evisimsrcSharpness:
		hrRet = ResultFromScode(E_FAIL);

		if (pVPA)
		{
			hrRet = pVPA->GetRange(VideoProcAmp_Sharpness, &lMin, &lMax, &dlStepping, &lDefault, &lFlags);
		}

		break;

	case evisimsrcGamma:
		hrRet = ResultFromScode(E_FAIL);

		if (pVPA)
		{
			hrRet = pVPA->GetRange(VideoProcAmp_Gamma, &lMin, &lMax, &dlStepping, &lDefault, &lFlags);
		}

		break;

	case evisimsrcColorEnable:
		hrRet = ResultFromScode(E_FAIL);

		if (pVPA)
		{
			hrRet = pVPA->GetRange(VideoProcAmp_ColorEnable, &lMin, &lMax, &dlStepping, &lDefault, &lFlags);
		}

		break;

	case evisimsrcWhiteBalance:
		hrRet = ResultFromScode(E_FAIL);

		if (pVPA)
		{
			hrRet = pVPA->GetRange(VideoProcAmp_WhiteBalance, &lMin, &lMax, &dlStepping, &lDefault, &lFlags);
		}

		break;

	case evisimsrcBacklightCompensation:
		hrRet = ResultFromScode(E_FAIL);

		if (pVPA)
		{
			hrRet = pVPA->GetRange(VideoProcAmp_BacklightCompensation, &lMin, &lMax, &dlStepping, &lDefault, &lFlags);
		}

		break;

	case evisimsrcPan:
		hrRet = ResultFromScode(E_FAIL);

		if (pCC)
		{
			hrRet = pCC->GetRange(CameraControl_Pan, &lMin, &lMax, &dlStepping, &lDefault, &lFlags);
		}

		break;

	case evisimsrcTilt:
		hrRet = ResultFromScode(E_FAIL);

		if (pCC)
		{
			hrRet = pCC->GetRange(CameraControl_Tilt, &lMin, &lMax, &dlStepping, &lDefault, &lFlags);
		}

		break;

	case evisimsrcRoll:
		hrRet = ResultFromScode(E_FAIL);

		if (pCC)
		{
			hrRet = pCC->GetRange(CameraControl_Roll, &lMin, &lMax, &dlStepping, &lDefault, &lFlags);
		}

		break;

	case evisimsrcZoom:
		hrRet = ResultFromScode(E_FAIL);

		if (pCC)
		{
			hrRet = pCC->GetRange(CameraControl_Zoom, &lMin, &lMax, &dlStepping, &lDefault, &lFlags);
		}

		break;

	case evisimsrcExposure:
		hrRet = ResultFromScode(E_FAIL);

		if (pCC)
		{
			hrRet = pCC->GetRange(CameraControl_Exposure, &lMin, &lMax, &dlStepping, &lDefault, &lFlags);
		}

		break;

	case evisimsrcIris:
		hrRet = ResultFromScode(E_FAIL);

		if (pCC)
		{
			hrRet = pCC->GetRange(CameraControl_Iris, &lMin, &lMax, &dlStepping, &lDefault, &lFlags);
		}

		break;

	case evisimsrcFocus:
		hrRet = ResultFromScode(E_FAIL);

		if (pCC)
		{
			hrRet = pCC->GetRange(CameraControl_Focus, &lMin, &lMax, &dlStepping, &lDefault, &lFlags);
		}

		break;

	default:
		hrRet = ResultFromScode(E_FAIL);
		break;
	}

	if (plMin != 0)
		*plMin = lMin;
	if (plMax != 0)
		*plMax = lMax;
	if (pdlStepping != 0)
		*pdlStepping = dlStepping;
	if (plDefault != 0)
		*plDefault = lDefault;
	if (plFlags != 0)
		*plFlags = lFlags;

	return hrRet;
}

STDMETHODIMP CVisDSCamera::GetImSrcValue(THIS_ EVisImSrc evisimsrc, LONG *plValue)
		CONST
{
	HRESULT hrRet = ResultFromScode(S_OK);
	LONG lValue;
	LONG flags;

	switch (evisimsrc)
	{
	case evisimsrcFContinuousGrab:
		lValue = ((m_fInContinuousGrab) ? 1 : 0);
		break;

	case evisimsrcFReflectVert:
		lValue = ((m_fFlippedImage) ? 1 : 0);
		hrRet = ResultFromScode(S_OK);
		break;

	case evisimsrcWidth:
		lValue = (int) m_ImageWidth;
		break;

	case evisimsrcHeight:
		lValue = (int) m_ImageHeight;
		break;

	case evisimsrcImEncoding:
		lValue = m_evisimencoding;
		break;

	case evisimsrcNBuffers:
		//CHECK: 1) Why do we need this. 2)Does CAPSTATUS!wNumAllocated returns what we want?
		lValue = 1;
		break;

	case evisimsrcBrightness:
		hrRet = ResultFromScode(E_FAIL);

		if (pVPA)
		{
			hrRet = pVPA->Get(VideoProcAmp_Brightness, &lValue, &flags);
		}

		break;

	case evisimsrcContrast:
		hrRet = ResultFromScode(E_FAIL);

		if (pVPA)
		{
			hrRet = pVPA->Get(VideoProcAmp_Contrast, &lValue, &flags);
		}

		break;

	case evisimsrcHue:
		hrRet = ResultFromScode(E_FAIL);

		if (pVPA)
		{
			hrRet = pVPA->Get(VideoProcAmp_Hue, &lValue, &flags);
		}

		break;

	case evisimsrcSaturation:
		hrRet = ResultFromScode(E_FAIL);

		if (pVPA)
		{
			hrRet = pVPA->Get(VideoProcAmp_Saturation, &lValue, &flags);
		}

		break;

	case evisimsrcSharpness:
		hrRet = ResultFromScode(E_FAIL);

		if (pVPA)
		{
			hrRet = pVPA->Get(VideoProcAmp_Sharpness, &lValue, &flags);
		}

		break;

	case evisimsrcGamma:
		hrRet = ResultFromScode(E_FAIL);

		if (pVPA)
		{
			hrRet = pVPA->Get(VideoProcAmp_Gamma, &lValue, &flags);
		}

		break;

	case evisimsrcColorEnable:
		hrRet = ResultFromScode(E_FAIL);

		if (pVPA)
		{
			hrRet = pVPA->Get(VideoProcAmp_ColorEnable, &lValue, &flags);
		}

		break;

	case evisimsrcWhiteBalance:
		hrRet = ResultFromScode(E_FAIL);

		if (pVPA)
		{
			hrRet = pVPA->Get(VideoProcAmp_WhiteBalance, &lValue, &flags);
		}

		break;

	case evisimsrcBacklightCompensation:
		hrRet = ResultFromScode(E_FAIL);

		if (pVPA)
		{
			hrRet = pVPA->Get(VideoProcAmp_BacklightCompensation, &lValue, &flags);
		}

		break;

	case evisimsrcPan:
		hrRet = ResultFromScode(E_FAIL);

		if (pCC)
		{
			hrRet = pCC->Get(CameraControl_Pan, &lValue, &flags);
		}

		break;

	case evisimsrcTilt:
		hrRet = ResultFromScode(E_FAIL);

		if (pCC)
		{
			hrRet = pCC->Get(CameraControl_Tilt, &lValue, &flags);
		}

		break;

	case evisimsrcRoll:
		hrRet = ResultFromScode(E_FAIL);

		if (pCC)
		{
			hrRet = pCC->Get(CameraControl_Roll, &lValue, &flags);
		}

		break;

	case evisimsrcZoom:
		hrRet = ResultFromScode(E_FAIL);

		if (pCC)
		{
			hrRet = pCC->Get(CameraControl_Zoom, &lValue, &flags);
		}

		break;

	case evisimsrcExposure:
		hrRet = ResultFromScode(E_FAIL);

		if (pCC)
		{
			hrRet = pCC->Get(CameraControl_Exposure, &lValue, &flags);
		}

		break;

	case evisimsrcIris:
		hrRet = ResultFromScode(E_FAIL);

		if (pCC)
		{
			hrRet = pCC->Get(CameraControl_Iris, &lValue, &flags);
		}

		break;

	case evisimsrcFocus:
		hrRet = ResultFromScode(E_FAIL);

		if (pCC)
		{
			hrRet = pCC->Get(CameraControl_Focus, &lValue, &flags);
		}

		break;

	default:
		lValue = 0;
		hrRet = ResultFromScode(E_FAIL);
		break;
	}

	if (plValue != 0)
		*plValue = lValue;

	return hrRet;
}


STDMETHODIMP CVisDSCamera::SetImSrcValue(THIS_ EVisImSrc evisimsrc, LONG lValue,
		LONG lFlags)
{
	// LATER:  Handle the manual configuration flag differently?
	HRESULT hrRet;

	switch (evisimsrc)
	{
	case evisimsrcFContinuousGrab:
		if (m_fInContinuousGrab)
		{
			if (lValue)
			{
				hrRet = ResultFromScode(S_FALSE);
			}
			else
			{
				// Stop capture.
				m_fInContinuousGrab = false;
				REFERENCE_TIME start = MAX_TIME;
    
				hrRet = pBuilder->ControlStream(&PIN_CATEGORY_CAPTURE, NULL, &start,
                                                                NULL, 0, 0);

			}
		}
		else
		{
			if (!lValue)
			{
				hrRet = ResultFromScode(S_FALSE);
			}
			else
			{
				// Start capture.
				m_fInContinuousGrab = true;

				REFERENCE_TIME stop = MAX_TIME;
				hrRet = pBuilder->ControlStream(&PIN_CATEGORY_CAPTURE, NULL, NULL,
                                                                &stop, 0, 0);

				if (FAILED(hrRet)){
					m_fInContinuousGrab = false;
				}

				if (m_fInContinuousGrab)
				{
					m_evisimencodingGrabCur = m_evisimencoding;
				}
				else
				{
					hrRet = ResultFromScode(E_FAIL);
				}
			}
		}
		break;

	case evisimsrcFReflectVert:
		hrRet = ResultFromScode(E_FAIL);
		if (m_fUseNegHtToFlip)
		{
			if (m_fFlippedImage)
			{
				if (lValue)
				{
					hrRet = ResultFromScode(S_FALSE);
				}
				else
				{
					AM_MEDIA_TYPE * pmt = PmtGet();
					if (!pmt) {
						hrRet = ResultFromScode(S_FALSE);
					} else {

						BITMAPINFOHEADER *pbmih = GetBmihFromMT(pmt);
					
						if (pbmih != 0)
						{
							assert(pbmih->biHeight >= 0);
							pbmih->biHeight = - pbmih->biHeight;

							if ((m_fCanSetVideoFormat)
									&& (pVSC->SetFormat(pmt) == NOERROR))
							{
								FreePmt(pmt);
								pmt = PmtGet();
								pbmih = GetBmihFromMT(pmt);

								if (pbmih->biHeight < 0)
								{
									m_fFlippedImage = false;
									hrRet = ResultFromScode(S_OK);
								}
							}
						}
						FreePmt(pmt);
					}
				}
			}
			else
			{
				if (lValue == 0)
				{
					hrRet = ResultFromScode(S_FALSE);
				}
				else
				{
					AM_MEDIA_TYPE * pmt = PmtGet();
					if (!pmt) {
						hrRet = ResultFromScode(S_FALSE);
					} else {

						BITMAPINFOHEADER *pbmih = GetBmihFromMT(pmt);

						if (pbmih != 0)
						{
							assert(pbmih->biHeight <= 0);
							pbmih->biHeight = - pbmih->biHeight;

							if ((m_fCanSetVideoFormat)
									&& (pVSC->SetFormat(pmt) == NOERROR))
							{
								FreePmt(pmt);
								pmt = PmtGet();
								pbmih = GetBmihFromMT(pmt);
								if (pbmih->biHeight > 0)
								{
									m_fFlippedImage = true;
									hrRet = ResultFromScode(S_OK);
								}
							}
						}		
						FreePmt(pmt);
					}
				}
			}
		}
		break;

	case evisimsrcWidth:
		hrRet = ResultFromScode(E_FAIL);
		if (lValue >= 0)
		{
			AM_MEDIA_TYPE * pmt = PmtGet();
			if (!pmt) {
				hrRet = ResultFromScode(S_FALSE);
			} else {

				BITMAPINFOHEADER *pbmih = GetBmihFromMT(pmt);

				if (pbmih != 0)
				{
					pbmih->biWidth = lValue;

					// UNDONE:  Often, users will attempt to set the width
					// and height with a pair of calls to SetImSrcValue.  It
					// might be the case that SetFormat will only succeed if
					// the width to height ratio is (something like) 4:3.
					// So, we may want to add a case to attempt to change both
					// the width and height here if SetFormat fails when we
					// try to use a change width to height ratio.
					if ((m_fCanSetVideoFormat)
							&& (pVSC->SetFormat(pmt) == NOERROR))
					{
						FreePmt(pmt);
						pmt = PmtGet();
						pbmih = GetBmihFromMT(pmt);
						if (pbmih->biWidth == lValue)
						{
							hrRet = ResultFromScode(S_OK);
						}
					}
				}				
				FreePmt(pmt);
			}
		}

		UpdateCachedInfo();
		break;

	case evisimsrcHeight:
		hrRet = ResultFromScode(E_FAIL);
		if (lValue >= 0)
		{
			AM_MEDIA_TYPE * pmt = PmtGet();
			if (!pmt) {
				hrRet = ResultFromScode(S_FALSE);
			} else {

				BITMAPINFOHEADER *pbmih = GetBmihFromMT(pmt);

				if (pbmih != 0)
				{
					if (pbmih->biHeight >= 0)
						pbmih->biHeight = lValue;
					else
						pbmih->biHeight = - lValue;

					// UNDONE:  Often, users will attempt to set the width
					// and height with a pair of calls to SetImSrcValue.  It
					// might be the case that SetFormat will only succeed if
					// the width to height ratio is (something like) 4:3.
					// So, we may want to add a case to attempt to change both
					// the width and height here if SetFormat fails when we
					// try to use a change width to height ratio.
					if ((m_fCanSetVideoFormat)
							&& (pVSC->SetFormat(pmt) == NOERROR))
					{
						FreePmt(pmt);
						pmt = PmtGet();
						pbmih = GetBmihFromMT(pmt);
						if ((pbmih->biHeight == lValue)
								|| (pbmih->biHeight == - lValue))
						{
							hrRet = ResultFromScode(S_OK);
						}
					}
				}	
				FreePmt(pmt);
			}
		}

		UpdateCachedInfo();
		break;

	case evisimsrcImEncoding:
		hrRet = ResultFromScode(E_FAIL);

		if ((lValue != evisimencodingUnknown)
				&& (lValue >= evisimencodingFirst)
				&& (lValue < evisimencodingLim)
				&& (m_rgfHaveImEncoding[lValue]))
		{
			AM_MEDIA_TYPE * pmt = PmtGet();
			if (!pmt) {
				hrRet = ResultFromScode(S_FALSE);
			} else {

				BITMAPINFOHEADER *pbmih = GetBmihFromMT(pmt);

				if (pbmih != 0)
				{
					if (lValue == ImEncodingGetDwFourCC(pbmih->biCompression,
									pbmih->biBitCount))
					{
						hrRet = ResultFromScode(S_FALSE);
					}
					else
					{
						pbmih->biBitCount = VisCbitGetImEncoding((EVisImEncoding) lValue);
						pbmih->biCompression = DwFourCCGetImEncoding((EVisImEncoding) lValue);
						pbmih->biClrUsed = 0;
						pbmih->biClrImportant = 0;

						if ((m_fCanSetVideoFormat)
								&& (pVSC->SetFormat(pmt) == NOERROR))
						{
							FreePmt(pmt);
							pmt = PmtGet();
							pbmih = GetBmihFromMT(pmt);
							if (lValue == ImEncodingGetDwFourCC(pbmih->biCompression,
									pbmih->biBitCount))
							{
								hrRet = ResultFromScode(S_OK);
							}
						}
					}
				}
				FreePmt(pmt);
			}
		}

		UpdateCachedInfo();
		break;

	case evisimsrcBrightness:
		hrRet = ResultFromScode(E_FAIL);

		if (pVPA)
		{
			hrRet = pVPA->Set(VideoProcAmp_Brightness, lValue, VideoProcAmp_Flags_Manual);
		}

		break;

	case evisimsrcContrast:
		hrRet = ResultFromScode(E_FAIL);

		if (pVPA)
		{
			hrRet = pVPA->Set(VideoProcAmp_Contrast, lValue, VideoProcAmp_Flags_Manual);
		}

		break;

	case evisimsrcHue:
		hrRet = ResultFromScode(E_FAIL);

		if (pVPA)
		{
			hrRet = pVPA->Set(VideoProcAmp_Hue, lValue, VideoProcAmp_Flags_Manual);
		}

		break;

	case evisimsrcSaturation:
		hrRet = ResultFromScode(E_FAIL);

		if (pVPA)
		{
			hrRet = pVPA->Set(VideoProcAmp_Saturation, lValue, VideoProcAmp_Flags_Manual);
		}

		break;

	case evisimsrcSharpness:
		hrRet = ResultFromScode(E_FAIL);

		if (pVPA)
		{
			hrRet = pVPA->Set(VideoProcAmp_Sharpness, lValue, VideoProcAmp_Flags_Manual);
		}

		break;

	case evisimsrcGamma:
		hrRet = ResultFromScode(E_FAIL);

		if (pVPA)
		{
			hrRet = pVPA->Set(VideoProcAmp_Gamma, lValue, VideoProcAmp_Flags_Manual);
		}

		break;

	case evisimsrcColorEnable:
		hrRet = ResultFromScode(E_FAIL);

		if (pVPA)
		{
			hrRet = pVPA->Set(VideoProcAmp_ColorEnable, lValue, VideoProcAmp_Flags_Manual);
		}

		break;

	case evisimsrcWhiteBalance:
		hrRet = ResultFromScode(E_FAIL);

		if (pVPA)
		{
			hrRet = pVPA->Set(VideoProcAmp_WhiteBalance, lValue, VideoProcAmp_Flags_Manual);
		}

		break;

	case evisimsrcBacklightCompensation:
		hrRet = ResultFromScode(E_FAIL);

		if (pVPA)
		{
			hrRet = pVPA->Set(VideoProcAmp_BacklightCompensation, lValue, VideoProcAmp_Flags_Manual);
		}

		break;

	case evisimsrcPan:
		hrRet = ResultFromScode(E_FAIL);

		if (pCC)
		{
			hrRet = pCC->Set(CameraControl_Pan, lValue, CameraControl_Flags_Manual);
		}

		break;

	case evisimsrcTilt:
		hrRet = ResultFromScode(E_FAIL);

		if (pCC)
		{
			hrRet = pCC->Set(CameraControl_Tilt, lValue, CameraControl_Flags_Manual);
		}

		break;

	case evisimsrcRoll:
		hrRet = ResultFromScode(E_FAIL);

		if (pCC)
		{
			hrRet = pCC->Set(CameraControl_Roll, lValue, CameraControl_Flags_Manual);
		}

		break;

	case evisimsrcZoom:
		hrRet = ResultFromScode(E_FAIL);

		if (pCC)
		{
			hrRet = pCC->Set(CameraControl_Zoom, lValue, CameraControl_Flags_Manual);
		}

		break;

	case evisimsrcExposure:
		hrRet = ResultFromScode(E_FAIL);

		if (pCC)
		{
			hrRet = pCC->Set(CameraControl_Exposure, lValue, CameraControl_Flags_Manual);
		}

		break;

	case evisimsrcIris:
		hrRet = ResultFromScode(E_FAIL);

		if (pCC)
		{
			hrRet = pCC->Set(CameraControl_Iris, lValue, CameraControl_Flags_Manual);
		}

		break;

	case evisimsrcFocus:
		hrRet = ResultFromScode(E_FAIL);

		if (pCC)
		{
			hrRet = pCC->Set(CameraControl_Focus, lValue, CameraControl_Flags_Manual);
		}

		break;

	default:
		hrRet = ResultFromScode(E_FAIL);
		break;
	}

	return hrRet;
}

STDMETHODIMP CVisDSCamera::IsPreferredImEncoding (THIS_
		EVisImEncoding evisimencoding) CONST
{
	if ((evisimencoding != evisimencodingUnknown)
			&& (evisimencoding >= evisimencodingFirst)
			&& (evisimencoding < evisimencodingLim)
			&& (m_rgfHaveImEncoding[evisimencoding]))
	{
		return ResultFromScode(S_OK);
	}

	return ResultFromScode(S_FALSE);
}

STDMETHODIMP CVisDSCamera::IsSupportedImEncoding (THIS_
		EVisImEncoding evisimencoding) CONST
{
	return IsPreferredImEncoding(evisimencoding);
}


STDMETHODIMP CVisDSCamera::HasDialog(THIS_ EVisVidDlg evisviddlg)
{
	HRESULT hrRet = ResultFromScode(S_FALSE);

	if ((pDlg != 0)
			&& (((evisviddlg == evisviddlgVFWDisplay)
					&& (pDlg->HasDialog(VfwCaptureDialog_Display)))
                || ((evisviddlg == evisviddlgVFWFormat)
					&& (pDlg->HasDialog(VfwCaptureDialog_Format)))
                || ((evisviddlg == evisviddlgVFWSource)
					&& (pDlg->HasDialog(VfwCaptureDialog_Source)))))
	{
		hrRet = ResultFromScode(S_OK);
	}
	else if (evisviddlg == evisviddlgVFWDisplay)
	{
		hrRet = HasPropertyPage(evisdsproppageCapFil);
	}
	else if (evisviddlg == evisviddlgVFWFormat)
	{
		hrRet = HasPropertyPage(evisdsproppageCapPin);
	}

	return hrRet;
}

STDMETHODIMP_(LONG) CVisDSCamera::DoDialog(THIS_ EVisVidDlg evisviddlg,
		HWND hwndParent)
{
	HRESULT hrRet = ResultFromScode(E_FAIL);
	if (HasDialog(evisviddlg) == ResultFromScode(S_OK))
	{
		switch (evisviddlg)
		{
			case evisviddlgVFWDisplay:
				 if (gvis_fPreferDirectShowDialogsToVFWDialogs)
					 hrRet = DoPropertyPage(evisdsproppageCapFil, hwndParent);

				 if ((hrRet != S_OK)
						&& (pDlg != 0)
						&& (pDlg->HasDialog(VfwCaptureDialog_Display)))
				 {
					 hrRet = pDlg->ShowDialog(VfwCaptureDialog_Display, hwndParent);
				 }

				 if ((hrRet != S_OK) && (!gvis_fPreferDirectShowDialogsToVFWDialogs))
					 hrRet = DoPropertyPage(evisdsproppageCapFil, hwndParent);

				 break;

			case evisviddlgVFWFormat:
				 if (gvis_fPreferDirectShowDialogsToVFWDialogs)
					 hrRet = DoPropertyPage(evisdsproppageCapPin, hwndParent);

				 if ((hrRet != S_OK)
						&& (pDlg != 0)
						&& (pDlg->HasDialog(VfwCaptureDialog_Format)))
				 {
					 hrRet = pDlg->ShowDialog(VfwCaptureDialog_Format, hwndParent);
				 }

				 if ((hrRet != S_OK) && (!gvis_fPreferDirectShowDialogsToVFWDialogs))
					 hrRet = DoPropertyPage(evisdsproppageCapPin, hwndParent);

				 break;
      
			case evisviddlgVFWSource:
				 if ((hrRet != S_OK)
						&& (pDlg != 0)
						&& (pDlg->HasDialog(VfwCaptureDialog_Source)))
				 {
					 hrRet = pDlg->ShowDialog(VfwCaptureDialog_Source, hwndParent);
				 }
				 break;
		}
		
		UpdateCachedInfo();
	}

    return hrRet;
}

STDMETHODIMP CVisDSCamera::HasPropertyPage(THIS_ EVisDSPropPage evisdsproppage)
{
    ISpecifyPropertyPages *pSpec;
    CAUUID cauuid;
	HRESULT hr = E_FAIL;

	switch (evisdsproppage) 
	{
	case evisdsproppageCapFil:
	    hr = pVCap->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pSpec);
		if (hr == S_OK) {
			hr = pSpec->GetPages(&cauuid);
	        if (hr == S_OK && cauuid.cElems > 0) {
		        CoTaskMemFree(cauuid.pElems);
			}
			pSpec->Release();
		}
		break;

	case evisdsproppageCapPin:
		IAMStreamConfig *pSC;
		hr = pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, pVCap,
				                   IID_IAMStreamConfig, (void **)&pSC);
	    if (hr == S_OK) {
		    hr = pSC->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pSpec);
			if (hr == S_OK) {
				hr = pSpec->GetPages(&cauuid);
				if (hr == S_OK && cauuid.cElems > 0) {
					CoTaskMemFree(cauuid.pElems);
				}
				pSpec->Release();
			}
	        pSC->Release();
		}
		break;
	}

	return hr;
}

STDMETHODIMP_(LONG) CVisDSCamera::DoPropertyPage(THIS_ EVisDSPropPage evisdsproppage,
		HWND hwndParent)
{
    ISpecifyPropertyPages *pSpec;
    CAUUID cauuid;
	HRESULT hr = E_FAIL;

	//We don't allow the property pages in between a single frame grab / continuous capture
	if (m_fInContinuousGrab || m_fInSingleFrameGrab) {
		return E_FAIL; //LATER: want to send out a better error code?
	}

	hr = pMC->Stop();
	if (FAILED(hr)) {
		OutputDebugString("Error : Cannot stop the graph");
		return hr;
	}

	hr = DisconnectFilters();
	if (FAILED(hr)) {
		return hr;
	}

	switch (evisdsproppage) 
	{
	case evisdsproppageCapFil:
	    hr = pVCap->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pSpec);
		if (hr == S_OK) {
			hr = pSpec->GetPages(&cauuid);
	        hr = OleCreatePropertyFrame(hwndParent, 30, 30, NULL, 1,
		                (IUnknown **)&pVCap, cauuid.cElems,
			            (GUID *)cauuid.pElems, 0, 0, NULL);
	        CoTaskMemFree(cauuid.pElems);
			pSpec->Release();
		}
		break;

	case evisdsproppageCapPin:
		IAMStreamConfig *pSC;
		hr = pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, pVCap,
				                   IID_IAMStreamConfig, (void **)&pSC);
	    if (hr == S_OK) {
		    hr = pSC->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pSpec);
			if (hr == S_OK) {
                    hr = pSpec->GetPages(&cauuid);
                    hr = OleCreatePropertyFrame(hwndParent, 30, 30, NULL, 1,
                    (IUnknown **)&pSC, cauuid.cElems,
                    (GUID *)cauuid.pElems, 0, 0, NULL);

                    CoTaskMemFree(cauuid.pElems);
                    pSpec->Release();
			}
	        pSC->Release();
		}
		break;
	}


	if (FAILED(hr)) {
		return hr; //LATER: debug message?
	}

	UpdateCachedInfo();

	hr = ConnectFilters();
	if (FAILED(hr)) {
		return hr; //LATER: debug message?
	}

	hr = pMC->Run();
	
	return hr;
}

		
// Attempt to specify a buffer to use to store image data.
STDMETHODIMP CVisDSCamera::AddBuffer(THIS_ void *pvBuffer,
		unsigned long cbBuffer, VisImSrcMemCallback pfnDeleteBuffer,
		void *pvDeleteBuffer)
{
	//LATER: I didn't find a way to specify buffers to DirectShow filter graphs. There is an IMemAllocator 
	//interface which might do exactly this. Investigate.
	return ResultFromScode(E_FAIL);
}

// CHECK: synchronize access?
// May wait until we get a new image if (imageMostRecent == imageCur).
// If the buffer was provided by the caller by using the AddBuffer call,
// the caller is responsible for freeing the buffer.
STDMETHODIMP CVisDSCamera::GrabNext(THIS)
{
	HRESULT hrRet;
	
	if (m_fInContinuousGrab)
	{
		// LATER:  Block until we get the next frame?
		hrRet = ResultFromScode(S_FALSE);
	}
	else
	{
		// Need to grab another frame.
		m_fInSingleFrameGrab = true;
		m_evisimencodingGrabCur = m_evisimencoding;

		REFERENCE_TIME start = MAX_TIME, stop = MAX_TIME;

		//LATER: figure out why specifying 0 for both start and stop doesn't work.
		hrRet = pBuilder->ControlStream(&PIN_CATEGORY_CAPTURE, NULL, NULL, &stop,
                                                                        0, 0);	
		VisWaitForSingleObject(m_SingleFrameSemaphore, INFINITE);

		hrRet = pBuilder->ControlStream(&PIN_CATEGORY_CAPTURE, NULL, &start, NULL,
                                                                        0, 0);	
		m_fInSingleFrameGrab = false;

		hrRet = ResultFromScode(S_OK);
	}

	return hrRet;
}


void CVisDSCamera::SetupCaptureParams(void)
{
    AM_MEDIA_TYPE *pmt;
    HRESULT hr = E_FAIL;
    if (pVSC) {
		hr = pVSC->GetFormat(&pmt);
		// DV capture does not use a VIDEOINFOHEADER
		if (hr == NOERROR) {
		    if (pmt->formattype == FORMAT_VideoInfo) {
				VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *)pmt->pbFormat;
				pvi->AvgTimePerFrame = (LONGLONG)(33333);
				pvi->dwBitErrorRate = 0;
				hr = pVSC->SetFormat(pmt);
		    }
		    DeleteMediaType(pmt);
		}
	}
	if (pVSC && hr != NOERROR)
		OutputDebugString("Cannot set frame rate for capture\n");
}


bool CVisDSCamera::FReadSettings(void)
{
	bool fRet = false;
	
	for (int i = 0; i < evisimencodingLim; ++i)
	{
		m_rgdwFourCCImEncoding[i] = 0;
		m_rgeflipImEncoding[i] = eflipUnknown;
		m_rgfHaveImEncoding[i] = false;
	}

	// Find the registry key.
	std::string strKey = VIS_SZ_REG_IMSRC_DEVS  "\\";
	strKey += g_szDSProviderID;
	strKey += "\\";
	strKey += SzName();

#ifdef LATER
	// This code is not needed now, but could be used to get an App ID
	// to use in app-specific keys.
	char szFullAppName[MAX_PATH];
	const char *szAppID = 0;
	if (GetModuleFileName(0, szFullAppName, sizeof(szFullAppName)) != 0)
	{
		szAppID = strrchr(szFullAppName, '\\');
		if (szAppID != 0)
			++ szAppID;
		else
			szAppID = szFullAppName;
	}
#endif // LATER

	// Attempt to open the reg key.
	HKEY hkey;
	if (!VisFOpenRegKey(&hkey, strKey.c_str()))
	{
		fRet = FFindImEncodings();
		UpdateCachedInfo(); //FFindImEncoding doesn't set the usenegheighttoflip and m_flipped values.
	}
	else
	{
		DWORD dwT;

		if (!VisFReadBoolFromReg(&m_fCanSetVideoFormat, hkey, "CanSetVideoFormat"))
		{
			// Default assumption is that we can set the video format.
			m_fCanSetVideoFormat = true;
		}

		if (!VisFReadBoolFromReg(&m_fFlippedImage, hkey, "Flipped"))
		{
			// Default assumption is that the image is flipped.
			m_fFlippedImage = true;
		}

		if (!VisFReadBoolFromReg(&m_fUseNegHtToFlip, hkey, "UseNegHeightToFlip"))
		{
			// Default assumption is that we can't use negative height to filp
			// images.
			m_fUseNegHtToFlip = false;
		}

		if ((VisFReadDwordFromReg(&dwT, hkey, "EVisConvYUV"))
				&& (dwT >= evisconvyuvFirst)
				&& (dwT < evisconvyuvLim))
		{
			m_evisconvyuv = (EVisConvYUV) dwT;
		}
		else
		{
			m_evisconvyuv = evisconvyuvDefault;
		}

		int dx;
		bool fHaveWidth = false;
		if (VisFReadIntFromReg(&dx, hkey, "Width")
				&& (dx > 0) && (dx < 5000))
		{
			fHaveWidth = true;
		}
		else
		{
			// Use 320 as the default width.
			dx = 320;
		}

		int dy;
		if (VisFReadIntFromReg(&dy, hkey, "Height")
				&& (dy > 0) && (dy < 5000))
		{
			if (!fHaveWidth)
				dx = 6400 * dy / 4800;
		}
		else if (fHaveWidth)
		{
			dy = 4800 * dx / 6400;
		}
		else
		{
			// Use 320 as the default width.
			dx = 320;
		}

		if ((VisFReadDwordFromReg(&dwT, hkey, "EVisImEncoding"))
				&& (dwT >= evisimencodingFirst)
				&& (dwT != evisimencodingUnknown)
				&& (dwT < evisimencodingLim))
		{
			m_evisimencoding = (EVisImEncoding) dwT;
		}

		// We'll need to get a BITMAPINFOHEADER to set the width, height,
		// and EVisImEncoding.
		AM_MEDIA_TYPE * pmt = PmtGet();
		BITMAPINFOHEADER *pbmih = NULL;
		if (!pmt || (pbmih = GetBmihFromMT(pmt)) == 0)
		{
			fRet = FFindImEncodings();
		}
		else
		{
			//arvindj: this seems more reasonable that checking biHeight-see VFW code
			if (pbmih->biCompression == BI_RGB || pbmih->biCompression == BI_BITFIELDS)
				m_fUseNegHtToFlip = true;

			bool fFoundImEncoding = false;
			std::string strVal;
			for (int i = evisimencodingFirst; i < evisimencodingLim; ++ i)
			{
				if (i == evisimencodingUnknown)
					continue;

				assert(VisSzGetImEncoding((EVisImEncoding) i) != 0);

				strVal = "EVisImEncoding";
				strVal += VisSzGetImEncoding((EVisImEncoding) i);
				if (VisFReadDwordFromReg(&dwT, hkey, strVal.c_str()))
				{
					fFoundImEncoding = true;
					m_rgfHaveImEncoding[i] = true;
					m_rgdwFourCCImEncoding[i] = dwT;

					strVal = "Flipped";
					strVal += VisSzGetImEncoding((EVisImEncoding) i);
					bool fFlipped;
					if (VisFReadBoolFromReg(&fFlipped, hkey, strVal.c_str()))
					{
						m_rgeflipImEncoding[i] = (fFlipped ? eflipYes : eflipNo);
					}
					else
					{
						m_rgeflipImEncoding[i] = eflipUnknown;
					}
				}
			}

			if (!fFoundImEncoding)
				fFoundImEncoding = FFindImEncodings();

			fRet = fFoundImEncoding;

			if (fRet)
			{
				bool fChangedVideoFormat = false;

				if (pbmih->biWidth != (DWORD) dx)
				{
					pbmih->biWidth = (DWORD) dx;
					fChangedVideoFormat = true;
				}

				if ((pbmih->biHeight != (DWORD) dy)
						&& (pbmih->biHeight != - (DWORD) dy))
				{
					if ((m_fUseNegHtToFlip) && (!m_fFlippedImage))
						pbmih->biHeight = - (DWORD) dy;
					else
						pbmih->biHeight = (DWORD) dy;
					fChangedVideoFormat = true;
				}

				if (m_fUseNegHtToFlip)
				{
					if (m_fFlippedImage)
					{
						if (((int) pbmih->biHeight) < 0)
						{
							pbmih->biHeight = - pbmih->biHeight;
							fChangedVideoFormat = true;
						}
					}
					else
					{
						if (((int) pbmih->biHeight) > 0)
						{
							pbmih->biHeight = - pbmih->biHeight;
							fChangedVideoFormat = true;
						}
					}
				}

				if (m_evisimencoding == evisimencodingUnknown)
				{
					m_evisimencoding = ImEncodingGetDwFourCC(
							pbmih->biCompression, pbmih->biBitCount);
				}
				else if (m_evisimencoding != ImEncodingGetDwFourCC(
						pbmih->biCompression, pbmih->biBitCount))
				{
					pbmih->biBitCount = VisCbitGetImEncoding(m_evisimencoding);
					pbmih->biCompression
							= DwFourCCGetImEncoding(m_evisimencoding);
					pbmih->biClrUsed = 0;
					pbmih->biClrImportant = 0;
					fChangedVideoFormat = true;
				}

				// Attempt to use the new settings.
				if ((m_fCanSetVideoFormat) && (fChangedVideoFormat))
				{
					pVSC->SetFormat(pmt);
				}
			}

		}
		// Clean up memory allocated above.
		FreePmt(pmt);
		pbmih = 0;

		VisFCloseRegKey(hkey);
	}

	return fRet;
}

// Set a callback function to be called when new frames arrive during
// a continuous grab operation.
STDMETHODIMP CVisDSCamera::GetFrameCallback(T_PfnVisImSrcFrameCallback *ppfnCallback,
		void **ppvUser) CONST
{
	if (ppfnCallback != 0)
		*ppfnCallback = m_pfnCallback;
	if (ppvUser != 0)
		*ppvUser = m_pvUser;

	return ResultFromScode(S_OK);
}

STDMETHODIMP CVisDSCamera::SetFrameCallback(T_PfnVisImSrcFrameCallback pfnCallback,
		void *pvUser)
{
	m_pfnCallback = pfnCallback;
	m_pvUser = pvUser;

	return ResultFromScode(S_OK);
}


LRESULT CALLBACK CVisDSCamera::StaticFrameCallbackProc(CVisDSCamera * pDS,
		IMediaSample * pMS)
{
	assert(pDS != 0);
	assert(pMS != 0);

	return pDS->FrameCallbackProc(pMS);
}

LRESULT CVisDSCamera::FrameCallbackProc(IMediaSample *pMS)
{
	HRESULT hr;
	assert(pMS != 0);

	int cb = min(pMS->GetSize(), pMS->GetActualDataLength());
	if (((m_fInContinuousGrab) || (m_fInSingleFrameGrab))
			&& (m_pfnCallback != 0)
			&& (cb > 0))
	{
		SVisImSrcFrame imsrcframe;
		ZeroMemory(&imsrcframe, sizeof(imsrcframe));

		imsrcframe.m_rect.left = 0;
		imsrcframe.m_rect.right = m_ImageWidth;
		imsrcframe.m_rect.top = 0;

		int dy = (int) (m_ImageHeight);
		if (dy < 0) dy *= -1;
		imsrcframe.m_rect.bottom = dy;

		assert(m_evisimencodingGrabCur != evisimencodingUnknown);
		if ((m_evisimencodingGrabCur != m_evisimencoding)
				&& (VisCbitGetImEncoding(m_evisimencodingGrabCur)
						!= VisCbitGetImEncoding(m_evisimencoding))
				&& ((cb * 8 / ((int) m_ImageWidth) / dy)
					== VisCbitGetImEncoding(m_evisimencoding)))
		{
			imsrcframe.m_evisimencoding = m_evisimencoding;
		}
		else
		{
			imsrcframe.m_evisimencoding = m_evisimencodingGrabCur;
		}

		int cbit = VisCbitGetImEncoding(imsrcframe.m_evisimencoding);
		if (cb * 8 < ((int) m_ImageWidth)
						* ((int) dy)
						* cbit)
		{
			imsrcframe.m_rect.bottom = imsrcframe.m_rect.top +
					(cb * 8 / ((int) m_ImageWidth) / cbit);
		}

		// LATER:  imsrcframe.m_filetime = ??? + lpVHdr->dmTimeCaptured;
		GetSystemTimeAsFileTime(&(imsrcframe.m_filetime));

		//bugbug: hr check?
		hr = pMS->GetPointer((unsigned char **)&imsrcframe.m_pvData);

		imsrcframe.m_cbData = cb;

		imsrcframe.m_evisconvyuv = m_evisconvyuv;

		// For the beta, we don't support color maps.
		// LATER:  We might also try getting the color table from palette
		// entries by using the HPALETTE in the CAPSTATUS data structure.
		// Or it might be part of the BITMAPINFO structure.
		imsrcframe.m_prgbqColormap = 0;
		imsrcframe.m_crgbqColormap = 0;

		imsrcframe.m_pfnDeleteData = 0;
		imsrcframe.m_pvDeleteData = 0;

		imsrcframe.dwReserved = 0;

		imsrcframe.m_dwFlags = evisfiminfVolatileData;

		if (m_rgeflipImEncoding[imsrcframe.m_evisimencoding] != eflipUnknown)
		{
			if (m_rgeflipImEncoding[imsrcframe.m_evisimencoding] == eflipYes)
				imsrcframe.m_dwFlags |= evisfiminfVertReflect;
		}
		else if (m_fFlippedImage)
		{
			imsrcframe.m_dwFlags |= evisfiminfVertReflect;
		}

		if (imsrcframe.m_rect.bottom > 0)
			m_pfnCallback((IVisImSrcDevice *) this, &imsrcframe, m_pvUser);

//		FreePmt(pbmi);
	}

	//Release the single frame grab semaphore
	if (m_fInSingleFrameGrab)
		ReleaseSemaphore(m_SingleFrameSemaphore, 1, 0);

	//check this out..we probably need HRESULTs
	return NOERROR;
}


BITMAPINFOHEADER *CVisDSCamera::GetBmihFromMT(AM_MEDIA_TYPE * pmt)
{
	//LATER - Do we accept format type Format_VideoInfo2? 
	if (pmt && pmt->formattype == FORMAT_VideoInfo) {
		return &((VIDEOINFOHEADER *)pmt->pbFormat)->bmiHeader;
	}

	return NULL;
}

AM_MEDIA_TYPE *CVisDSCamera::PmtGet(void) const
{
	AM_MEDIA_TYPE * pmt, * pmtd;

	pmtd = new AM_MEDIA_TYPE[1];

	if (pVSC->GetFormat(&pmt) != NOERROR) {
		return NULL;
	}
	CopyMediaType(pmtd, pmt);
	return pmtd;
}

	
void CVisDSCamera::FreePmt(AM_MEDIA_TYPE * pmt) const
{
	if (pmt != 0) {
		FreeMediaType( (AM_MEDIA_TYPE &)(*pmt));
		delete pmt;
	}
}

EVisImEncoding CVisDSCamera::EVisImEncodingGet(BITMAPINFOHEADER *pbmih,
		bool fUseCachedInfo)
{
	if ((!fUseCachedInfo)
			|| (m_evisimencoding == evisimencodingUnknown))
	{

		if (pbmih != 0)
		{
			m_evisimencoding = ImEncodingGetDwFourCC(pbmih->biCompression,
					pbmih->biBitCount);
#ifdef _DEBUG
			if (m_evisimencoding == evisimencodingUnknown)
			{
				char szBuf[255];
				sprintf(szBuf, "VisSDK: (VisXDS) %d-bit format %#010x (== '%c%c%c%c",
						pbmih->biBitCount, pbmih->biCompression,
						((char *) &(pbmih->biCompression))[3],
						((char *) &(pbmih->biCompression))[2],
						((char *) &(pbmih->biCompression))[1],
						((char *) &(pbmih->biCompression))[0]);
				OutputDebugString(szBuf);
				OutputDebugString("') is not known!\n");
			}
#endif // _DEBUG
		}

	}

	assert(m_evisimencoding >= evisimencodingFirst);
	assert(m_evisimencoding < evisimencodingLim);

	return m_evisimencoding;
}


void CVisDSCamera::UpdateCachedInfo(void)
{
	AM_MEDIA_TYPE * pmt = PmtGet();
	BITMAPINFOHEADER *pbmih = GetBmihFromMT(pmt);

	EVisImEncodingGet(pbmih, false);

	if (pbmih != 0)
	{
		//arvindj: this seems more reasonable that checking biHeight-see VFW code
		if (pbmih->biCompression == BI_RGB || pbmih->biCompression == BI_BITFIELDS)
			m_fUseNegHtToFlip = true;

		if (pbmih->biHeight >= 0)
		{
			if (m_fUseNegHtToFlip)
				m_fFlippedImage = true;
		}
		else
		{
			m_fFlippedImage = false;
		}

		m_ImageWidth = pbmih->biWidth;
		m_ImageHeight = pbmih->biHeight;
	}
	FreePmt(pmt);
}


bool CVisDSCamera::FFindImEncodings(void)
{
	bool fFoundImEncoding = false;
	std::string strVal;

	// Save the current video format.
	AM_MEDIA_TYPE * pmt = PmtGet();
	BITMAPINFOHEADER *pbmih = GetBmihFromMT(pmt);

	if (!pbmih){
		FreePmt(pmt);
		return FALSE;
	}

	BITMAPINFOHEADER bmihspare = *pbmih; //save the original here

	pbmih->biSize = sizeof(BITMAPINFOHEADER);
	pbmih->biPlanes = 1;
	pbmih->biSizeImage = 0;
	pbmih->biXPelsPerMeter = 3150;
	pbmih->biYPelsPerMeter = 3150;
	pbmih->biClrUsed = 0;
	pbmih->biClrImportant = 0;
	pbmih->biWidth = 320;
	pbmih->biHeight = 240;

	if (m_fCanSetVideoFormat)
	{
		// Try setting a garbage format.  If this succeeds, we can't determine
		// the supported video formats by looking at the capSetVideoFormat
		// return value.
		pbmih->biBitCount = 16;
		pbmih->biCompression = 'ZzzZ';

		if (pVSC->SetFormat(pmt) == NOERROR)
		{
			m_fCanSetVideoFormat = false;
		}
	}


	if (m_fCanSetVideoFormat)
	{
		// We can try different formats and check the capSetVideoFormat
		// return value to see if they are supported.
		for (int i = evisimencodingFirst; i < evisimencodingLim; ++ i)
		{
			if ((i == evisimencodingUnknown)
					|| (i == evisimencodingGray4bit)	// Same as color4bit
					|| (i == evisimencodingGray8bit)	// Same as color8bit
					|| (i == evisimencodingGray16bit)	// Same as RGB555
					|| (i == evisimencodingRGB565)		// Same as RGB555
					|| (i == evisimencodingRGB48)		// cbit is too large
					|| (i == evisimencodingRGBA64)		// cbit is too large
					|| (i == evisimencodingYUV48)		// cbit is too large
					|| (i == evisimencodingYUVA64)		// cbit is too large
					|| (i == evisimencodingY41P)		// Not supported - ignore
					|| (i == evisimencodingY41T)		// Not supported - ignore
					|| (i == evisimencodingCLJR)		// Not supported - ignore
					|| (i == evisimencodingCLPL)		// Not supported - ignore
					|| (i == evisimencodingY210)		// Not supported - ignore
					|| (i == evisimencodingY410)		// Not supported - ignore
					|| (i == evisimencodingYUVP))		// Not supported - ignore
			{
				continue;
			}

#ifdef TRY_MULTIPLE_SIZES
			for (int iSize = 0; iSize < 2; ++iSize)
			{
				if (iSize == 0)
				{
					pbmih->biWidth = 320;
					pbmih->biHeight = 240;
				}
				else
				{
					pbmih->biWidth = 160;
					pbmih->biHeight = 120;
				}
#endif // TRY_MULTIPLE_SIZES

				pbmih->biBitCount = VisCbitGetImEncoding((EVisImEncoding) i);
				pbmih->biCompression = VisDwFourCCGetImEncoding((EVisImEncoding) i);

				if (pVSC->SetFormat(pmt) == NOERROR)
				{
					fFoundImEncoding = true;
					m_rgfHaveImEncoding[i] = true;
					m_rgeflipImEncoding[i] = eflipUnknown;
					m_rgdwFourCCImEncoding[i] = pbmih->biCompression;

#ifdef TRY_MULTIPLE_SIZES
					// If we can set the video format with one size, we don't
					// need to try other sizes.
					break;
#endif // TRY_MULTIPLE_SIZES
				}

				// UNDONE:  Add special case code for the I420 and cyuv FourCC values.
				// (I420 is the same as IYUV and cyuv is UYVY flipped.)
#ifdef TRY_MULTIPLE_SIZES
			}
#endif // TRY_MULTIPLE_SIZES
		}
	}

	//get the original back
	*pbmih = bmihspare;

	// See if we recognize the current format.
	EVisImEncoding evisimencoding = VisImEncodingGetDwFourCC(
			pbmih->biCompression, pbmih->biBitCount);

	assert((evisimencoding >= evisimencodingFirst)
			&& (evisimencoding < evisimencodingLim));

	if (evisimencoding != evisimencodingUnknown)
	{
		fFoundImEncoding = true;
		m_rgfHaveImEncoding[evisimencoding] = true;
		m_rgeflipImEncoding[evisimencoding] = eflipUnknown;
		m_rgdwFourCCImEncoding[evisimencoding] = pbmih->biCompression;
	}
	else
	{
		FoundUnknownDwFourCC(pbmih->biCompression, (int) pbmih->biBitCount);
	}

	// Attempt to restore the video format.
	if (m_fCanSetVideoFormat)
	{
		pVSC->SetFormat(pmt);
	}
		
	FreePmt(pmt);

	return fFoundImEncoding;
}


EVisImEncoding CVisDSCamera::ImEncodingGetDwFourCC(DWORD dwFourCC, int cbit)
{
	EVisImEncoding evisimencoding = evisimencodingUnknown;

	for (int i = evisimencodingFirst; i < evisimencodingLim; ++ i)
	{
		if ((i == evisimencodingUnknown)
				|| (!m_rgfHaveImEncoding[i]))
		{
			continue;
		}

		if ((m_rgdwFourCCImEncoding[i] == dwFourCC)
				&& (VisCbitGetImEncoding((EVisImEncoding) i) == cbit))
		{
			evisimencoding = (EVisImEncoding) i;
			break;
		}
	}

	if (evisimencoding == evisimencodingUnknown)
	{
		evisimencoding = VisImEncodingGetDwFourCC(dwFourCC, cbit);

		if (evisimencoding != evisimencodingUnknown)
		{
			// Add this value to the list of known image encodings.
			m_rgfHaveImEncoding[evisimencoding] = true;
			m_rgdwFourCCImEncoding[evisimencoding] = dwFourCC;
			m_rgeflipImEncoding[evisimencoding] = eflipUnknown;
		}
		else
		{
			FoundUnknownDwFourCC(dwFourCC, cbit);
		}
	}

	return evisimencoding;
}


void CVisDSCamera::FoundUnknownDwFourCC(DWORD dwFourCC, int cbit)
{
	for (int iUnknown = 0; iUnknown < m_cdwFourCCUnknownCur; ++ iUnknown)
	{
		if ((m_rgdwFourCCUnknown[iUnknown] == dwFourCC)
				&& (m_rgcbitFourCCUnknown[iUnknown] == cbit))
		{
			break;
		}
	}

	if (iUnknown == m_cdwFourCCUnknownCur)
	{
		if (m_cdwFourCCUnknownCur + 1 < eidwFourCCUnknownLim)
		{
			++ m_cdwFourCCUnknownCur;
			m_rgdwFourCCUnknown[iUnknown] = dwFourCC;
			m_rgcbitFourCCUnknown[iUnknown] = cbit;
		}

#ifdef _DEBUG
		char szBuf[255];
		sprintf(szBuf,
				"VisSDK: (VisXDS) %d-bit format %#010x (== '%c%c%c%c",
				cbit, dwFourCC,
				((char *) &dwFourCC)[3],
				((char *) &dwFourCC)[2],
				((char *) &dwFourCC)[1],
				((char *) &dwFourCC)[0]);
		OutputDebugString(szBuf);
		OutputDebugString("') is not known!\n");
#endif // _DEBUG
	}
}


DWORD CVisDSCamera::DwFourCCGetImEncoding(EVisImEncoding evisimencoding) const
{
	DWORD dwRet = 0;
	
	if ((evisimencoding >= evisimencodingFirst)
			&& (evisimencoding != evisimencodingUnknown)
			&& (evisimencoding < evisimencodingLim))
	{
		if (m_rgfHaveImEncoding[evisimencoding])
			dwRet = m_rgdwFourCCImEncoding[evisimencoding];
		else
			dwRet = VisDwFourCCGetImEncoding(evisimencoding);
	}

	return dwRet;
}

// Make a graph builder object we can use for capture graph building
//
BOOL CVisDSCamera::MakeBuilder()
{
    // we have one already
    if (pBuilder)
        return TRUE;

    HRESULT hr = CoCreateInstance((REFCLSID)CLSID_CaptureGraphBuilder,
                        NULL, CLSCTX_INPROC, (REFIID)IID_ICaptureGraphBuilder,
                        (void **)&pBuilder);
    return (hr == NOERROR) ? TRUE : FALSE;
}

// Make a graph object we can use for capture graph building
//
BOOL CVisDSCamera::MakeGraph()
{
    // we have one already
    if (pFg)
        return TRUE;

    HRESULT hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC,
                               IID_IGraphBuilder, (LPVOID *)&pFg);
    return (hr == NOERROR) ? TRUE : FALSE;
}

HRESULT CVisDSCamera::ConnectFilters()
{
	IPin * pcapOut = NULL;
	IPin * ptrnsIn = NULL;
	HRESULT hr;

	hr = pVTrns->FindPin(L"Input", &ptrnsIn);
    if (hr != NOERROR) {
        OutputDebugString("Error : Cannot find input pin on render filter");
        goto Fail;
    }

	hr = pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, pVCap,
                                IID_IPin, (void **)&pcapOut);
    if (hr != NOERROR) {
        OutputDebugString("Error : Cannot find output pin on capture filter");
        goto Fail;
    }

	hr = pFg->ConnectDirect(pcapOut, ptrnsIn, NULL);
    if (hr != NOERROR) {
        OutputDebugString("Error : Cannot connect the two filters");
        goto Fail;
    }

Fail:
	if (pcapOut)
		pcapOut->Release();
	if (ptrnsIn) 
		ptrnsIn->Release();
	return hr;
}

HRESULT CVisDSCamera::DisconnectFilters()
{
	IPin * pcapOut = NULL;
	IPin * ptrnsIn = NULL;
	HRESULT hr;

	hr = pVTrns->FindPin(L"Input", &ptrnsIn);
    if (hr != NOERROR) {
        OutputDebugString("Error : Cannot find input pin on render filter");
        goto Fail;
    }

	hr = pFg->Disconnect(ptrnsIn);
	ptrnsIn->Release();
    if (hr != NOERROR) {
        OutputDebugString("Error : Cannot disconnect the renderer's input pin");
        goto Fail;
    }

	hr = pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, pVCap,
                                IID_IPin, (void **)&pcapOut);
    if (hr != NOERROR) {
        OutputDebugString("Error : Cannot find output pin on capture filter");
        goto Fail;
    }

	hr = pFg->Disconnect(pcapOut);	
	pcapOut->Release();
    if (hr != NOERROR) {
        OutputDebugString("Error : Cannot disconnect the capture filter's output pin");
        goto Fail;
    }

Fail:
	return hr;
}