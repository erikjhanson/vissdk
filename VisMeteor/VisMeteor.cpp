// VisMeteor.cpp
//
// Copyright © 1997-2000 Microsoft Corporation, All Rights Reserved


// LATER:  If the provider kept a list of the MIL_IDs for each system (card),
// we might be able to support more than one digitizer (channel and signal
// combination) on the same card at the same time.


// UNDONE:  We seem to have a problem freeing a buffer when used in the ExCamera
// example program.  When the image sequence is destroyed in the ExCamera program,
// there is still a reference to the image source.  (When continuous grab is
// toggled in the ExCamera program, the CVisMeteor ref count is corrected.  We
// should spend more time checking what happens when continuous grab is toggled.)


#include "VisMeteorPch.h"
#include "VisMeteor.h"

#if M_MIL_USE_METEOR_II
#include "VisMeteorSpecific.h"
#endif

#include <process.h>

#include "string.h" // For ichNil


CVisMeteor::CVisMeteor(class CVisMeteorProvider *pProvider, int iDevice,
		int iSys, const char *szDig, int iChannel)
  : m_cRef(1),
	m_pProvider(pProvider),
	m_iDevice(iDevice),
	m_iSys(iSys),
	m_szDig(0),
	m_iChannel(iChannel),
	m_pfnCallback(0),
	m_pvUser(0),
	m_fInContinuousGrab(true),
	m_fInSingleFrameGrab(false),
	m_evisimencoding(evisimencodingRGBA32),
	m_fColorEnable(true),
	m_fPreserveAspect(true),
	m_hThread(0),
	m_dwThreadID(0),
	m_milidSys(M_NULL),
	m_milidDig(M_NULL),
	m_milidDisp(M_NULL),
	m_pMeteorBufCur(M_NULL),
	m_pMeteorBufNext(M_NULL),
	m_pfnGrabEndHandler(M_NULL),
	m_pvGrabEndHandler(0),
	m_pfnGrabStartHandler(M_NULL),
	m_pvGrabStartHandler(0),
	m_dblScale(0),
	m_dblScaleX(0),
	m_dblScaleY(0),
	m_fGrabFields(false)
{
	assert(m_pProvider != 0);
	m_pProvider->AddRef();

	InitializeCriticalSection(&m_criticalsection);

	m_filetimeLastGrab.dwLowDateTime = 0;
	m_filetimeLastGrab.dwHighDateTime = 0;

	// Try to store a copy of the digitizer string.
	// (Init below will fail if new fails here.)
	assert(szDig != 0);
	m_szDig = new char [strlen(szDig) + 1];
	if (m_szDig != 0)
		strcpy(m_szDig, szDig);

	m_strID = g_szMeteorProviderID;
	m_strID += ":  ";
	assert(ecchProviderSep == 3);

	char szT[256];
	*szT = 0;

	m_strID += "Card ";

	szT[0] = '0' + iSys;
	szT[1] = 0;
	m_strID += szT;

	m_strID += ", ";
	m_strID += szDig;

	if (iChannel == 0)
	{
		m_strID += ", Default Channel";
	}
	else
	{
		assert((iChannel > 0) && (iChannel <= 4));

		m_strID += ", Channel ";

		szT[0] = '0' + iChannel - 1;
		szT[1] = 0;
		m_strID += szT;
	}

	if (FGetDigitizer())
		ReadSettings();

	// LATER:  Should we use a background thread to handle GrabEnd callbacks?
}


CVisMeteor::~CVisMeteor(void)
{
	if (m_fInContinuousGrab)
	{
		// Stop continuous grab and wait for grabbing to end.
		m_fInContinuousGrab = false;

#ifdef TAKOUT
		MdigGrabWait(m_milidDig, M_GRAB_END);
		assert(m_pMeteorBufNext == 0);

		if (m_pMeteorBufCur != M_NULL);
			MdigGrabWait(m_milidDig, M_GRAB_END);

		assert(m_pMeteorBufCur == 0);
#endif // TAKEOUT
	}

	if (m_hThread != 0)
	{
		// Kill it.
		::PostThreadMessage(m_dwThreadID, WM_QUIT, 0, 0);
		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);
		m_hThread = 0;
	}

	m_pProvider->MeteorBufList().RemoveBufs(this);

	if (m_milidSys != M_NULL)
	{
		if (m_milidDig != M_NULL)
		{
			MdigHookFunction(m_milidDig, M_FIELD_START, m_pfnGrabStartHandler,
					m_pvGrabStartHandler);
			m_pfnGrabStartHandler = 0;
			m_pvGrabStartHandler = 0;

			MdigHookFunction(m_milidDig, M_GRAB_END, m_pfnGrabEndHandler,
					m_pvGrabEndHandler);
			m_pfnGrabEndHandler = 0;
			m_pvGrabEndHandler = 0;

			MdigFree(m_milidDig);
		}

		if (m_milidDisp != M_NULL)
		{
			MdispFree(m_milidDisp);
		}

		MdigFree(m_milidSys);
	}

	m_pProvider->Release();

	if (m_szDig != 0)
	{
		delete[] m_szDig;
		m_szDig = 0;
	}

	DeleteCriticalSection(&m_criticalsection);
}

STDMETHODIMP CVisMeteor::QueryInterface(THIS_ REFIID riid, void **ppvObject)
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
	else
	{
		*ppvObject = 0;
		hrRet = ResultFromScode(E_NOINTERFACE);
	}

	return hrRet;
}

STDMETHODIMP_(ULONG) CVisMeteor::AddRef(THIS)
{
	assert(m_cRef > 0);

	return ++m_cRef;
}

STDMETHODIMP_(ULONG) CVisMeteor::Release(THIS)
{
	unsigned long cRef = --m_cRef;

	if (cRef == 0)
		delete this;

	return cRef;
}

// Returns S_OK if successful.
HRESULT CVisMeteor::Init(void)
{
	return ResultFromScode(((m_milidDig != M_NULL) && (m_milidDisp != M_NULL))
			? S_OK : E_FAIL);
}


// Get a string identifying this device.
// Returns the number of bytes copied in pcb if successful.
STDMETHODIMP CVisMeteor::GetSzID(THIS_ char *sz, LONG *pcb)
{
	return VisGetSzPcbFromSz(sz, pcb, m_strID.c_str());
}

STDMETHODIMP CVisMeteor::GetSzName(THIS_ char *sz, LONG *pcb)
{
	int cchPrefix = strlen(g_szMeteorProviderID) + ecchProviderSep;
	assert(*(m_strID.c_str() + strlen(g_szMeteorProviderID)) == ':');
	assert(*(m_strID.c_str() + cchPrefix - 1) == ' ');
	assert(*(m_strID.c_str() + cchPrefix) != ' ');

	return VisGetSzPcbFromSz(sz, pcb, m_strID.c_str() + cchPrefix);
}

STDMETHODIMP CVisMeteor::SaveSettings(THIS_ bool fForAllApps)
{
	// UNDONE
	HRESULT hrRet = ResultFromScode(E_NOTIMPL);

	return hrRet;
}


// Properties of this image source.
STDMETHODIMP CVisMeteor::GetImSrcRange(THIS_ EVisImSrc evisimsrc, LONG *plMin,
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
	case evisimsrcImEncoding:
		lDefault = evisimencodingRGBA32;
		lMax = evisimencodingLim - 1;
		lFlags = evisimsrcflagAuto;
		break;

	case evisimsrcWidth:
		lDefault = m_sizeBaseline.cx / 2;
		lMax = m_sizeBaseline.cx;
		dlStepping = 4;
		lFlags = evisimsrcflagAuto;
		break;

	case evisimsrcHeight:
		lDefault = m_sizeBaseline.cy / 2;
		lMax = m_sizeBaseline.cy;
		lFlags = evisimsrcflagAuto;
		break;

	case evisimsrcXOffset:
		lDefault = 0;
		lMax = m_sizeBaseline.cx;
		lFlags = evisimsrcflagAuto;
		break;

	case evisimsrcYOffset:
		lDefault = 0;
		lMax = m_sizeBaseline.cy;
		lFlags = evisimsrcflagAuto;
		break;

	case evisimsrcBrightness:
		lDefault = m_lRefBlackDefault;
		lMin = M_MIN_LEVEL;
		lMax = M_MAX_LEVEL;
		lFlags = evisimsrcflagAuto;
		break;

	case evisimsrcSharpness:
		lDefault = m_lRefSharpnessDefault;
		lMin = M_MIN_LEVEL;
		lMax = M_MAX_LEVEL;
		lFlags = evisimsrcflagAuto;
		break;

	case evisimsrcContrast:
		lDefault = m_lRefContrastDefault;
		lMin = M_MIN_LEVEL;
		lMax = M_MAX_LEVEL;
		lFlags = evisimsrcflagAuto;
		break;

	case evisimsrcHue:
		lDefault = m_lRefHueDefault;
		lMin = M_MIN_LEVEL;
		lMax = M_MAX_LEVEL;
		lFlags = evisimsrcflagAuto;
		break;

	case evisimsrcSaturation:
		lDefault = m_lRefSaturationDefault;
		lMin = M_MIN_LEVEL;
		lMax = M_MAX_LEVEL;
		lFlags = evisimsrcflagAuto;
		break;

	case evisimsrcWhiteBalance:
		lDefault = m_lRefWhiteDefault;
		lMin = M_MIN_LEVEL;
		lMax = M_MAX_LEVEL;
		lFlags = evisimsrcflagAuto;
		break;

	case evisimsrcColorEnable:
		lDefault = 1;
		lMax = 1;
		lFlags = evisimsrcflagAuto;
		break;

	case evisimsrcNBuffers:
		lDefault = 0;
		lMax = 20;
		lFlags = evisimsrcflagAuto;
		break;

	// Device Specific Flags
#if M_MIL_USE_METEOR_II
	case evisimsrcMeteorfPreserveAspect:
		lDefault = 1;
		lMax = 1;
		lFlags = evisimsrcflagAuto;
		break;
#endif

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

STDMETHODIMP CVisMeteor::GetImSrcValue(THIS_ EVisImSrc evisimsrc, LONG *plValue)
		CONST
{
	HRESULT hrRet = ResultFromScode(S_OK);
	LONG lValue;

	switch (evisimsrc)
	{
	case evisimsrcFContinuousGrab:
		lValue = ((m_fInContinuousGrab) ? 1 : 0);
		break;

	case evisimsrcImEncoding:
		lValue = m_evisimencoding;
		break;

	case evisimsrcWidth:
		lValue = m_size.cx;
		break;

	case evisimsrcHeight:
		lValue = m_size.cy;
		break;

	case evisimsrcXOffset:
		lValue = MdigInquire(m_milidDig, M_DIG_SOURCE_OFFSET_X, M_NULL);
		break;

	case evisimsrcYOffset:
		lValue = MdigInquire(m_milidDig, M_DIG_SOURCE_OFFSET_Y, M_NULL);
		break;

	case evisimsrcBrightness:
		// LATER:  Specify the channel if not the default channel?
		// UNDONE:  Use M_DIG_REF_BRIGHTNESS for evisimsrcSharpness?
		lValue = MdigInquire(m_milidDig, M_DIG_REF_BLACK, M_NULL);
		break;

	case evisimsrcSharpness:
		lValue = MdigInquire(m_milidDig, M_DIG_BRIGHTNESS_REF, M_NULL);
		break;

	case evisimsrcContrast:
		lValue = MdigInquire(m_milidDig, M_DIG_CONTRAST_REF, M_NULL);
		break;

	case evisimsrcHue:
		lValue = MdigInquire(m_milidDig, M_DIG_HUE_REF, M_NULL);
		break;

	case evisimsrcSaturation:
		lValue = MdigInquire(m_milidDig, M_DIG_SATURATION_REF, M_NULL);
		break;

	case evisimsrcWhiteBalance:
		// LATER:  Specify the channel if not the default channel?
		lValue = MdigInquire(m_milidDig, M_DIG_REF_WHITE, M_NULL);
		break;

	case evisimsrcColorEnable:
		lValue = (m_fColorEnable ? 1 : 0);
		break;

	case evisimsrcNBuffers:
		lValue = m_pProvider->MeteorBufList().CBufCur(this);
		break;


	//Device Specific Values
#if M_MIL_USE_METEOR_II
	case evisimsrcMeteorfPreserveAspect:
		lValue = m_fPreserveAspect;
		break;
#endif

	default:
		lValue = 0;
		hrRet = ResultFromScode(E_FAIL);
		break;
	}

	if (plValue != 0)
		*plValue = lValue;

	return hrRet;
}


STDMETHODIMP CVisMeteor::SetImSrcValue(THIS_ EVisImSrc evisimsrc, LONG lValue,
		LONG lFlags)
{
	HRESULT hrRet = ResultFromScode(S_OK);

	switch (evisimsrc)
	{
	case evisimsrcFContinuousGrab:
		m_fInContinuousGrab = !m_fInContinuousGrab;
		hrRet = ResultFromScode(S_OK);

		if (m_fInContinuousGrab)
		{
			// Start grabbing.
			GrabNext();
		}
		break;

	case evisimsrcImEncoding:
		if (IsPreferredImEncoding((EVisImEncoding) lValue)
				== ResultFromScode(S_OK))
		{
			m_evisimencoding = (EVisImEncoding) lValue;
		}
		else
		{
			hrRet = ResultFromScode(E_FAIL);
		}
		break;

	case evisimsrcWidth:
		// Round to next hightest multiple of four.
		lValue = (((lValue + 3) >> 2) << 2);

		if (lValue <= 0)
		{
			hrRet = ResultFromScode(E_FAIL);
		}
		else if (lValue > m_sizeBaseline.cx)
		{
			hrRet = ResultFromScode(S_FALSE);
			m_size.cx = m_sizeBaseline.cx;
		}
		else
		{
			m_size.cx = lValue;
		}

		CheckScales();
		CheckFrameOrFieldGrab();

		break;

	case evisimsrcHeight:
		if (lValue <= 0)
		{
			hrRet = ResultFromScode(E_FAIL);
		}
		else if (lValue > m_sizeBaseline.cy)
		{
			hrRet = ResultFromScode(S_FALSE);
			m_size.cy = m_sizeBaseline.cy;
		}
		else
		{
			m_size.cy = lValue;
		}

		CheckScales();
		CheckFrameOrFieldGrab();

		break;

	case evisimsrcXOffset:
		// Round to next hightest multiple of four.
		lValue = (((lValue + 3) >> 2) << 2);

		// UNDONE:  Grab fields if width and height are less than half baseline.
		if ((lValue <= 0) || (lValue > m_sizeBaseline.cy))
			hrRet = ResultFromScode(E_FAIL);
		else
			MdigControl(m_milidDig, M_DIG_SOURCE_OFFSET_X, lValue);
		break;

	case evisimsrcYOffset:
		// UNDONE:  Grab fields if width and height are less than half baseline.
		if ((lValue <= 0) || (lValue > m_sizeBaseline.cy))
			hrRet = ResultFromScode(E_FAIL);
		else
			MdigControl(m_milidDig, M_DIG_SOURCE_OFFSET_Y, lValue);
		break;

	case evisimsrcBrightness:
		// LATER:  Specify the channel if not the default channel?
		// UNDONE:  Use M_DIG_REF_BRIGHTNESS for evisimsrcSharpness?
		assert(((lValue >= M_MIN_LEVEL) && (lValue <= M_MAX_LEVEL))
				|| (lValue == M_DEFAULT));
		MdigReference(m_milidDig, M_BLACK_REF, lValue);
		break;

	case evisimsrcSharpness:
		assert(((lValue >= M_MIN_LEVEL) && (lValue <= M_MAX_LEVEL))
				|| (lValue == M_DEFAULT));
		MdigReference(m_milidDig, M_BRIGHTNESS_REF, lValue);
		break;

	case evisimsrcContrast:
		assert(((lValue >= M_MIN_LEVEL) && (lValue <= M_MAX_LEVEL))
				|| (lValue == M_DEFAULT));
		MdigReference(m_milidDig, M_CONTRAST_REF, lValue);
		break;

	case evisimsrcHue:
		assert(((lValue >= M_MIN_LEVEL) && (lValue <= M_MAX_LEVEL))
				|| (lValue == M_DEFAULT));
		MdigReference(m_milidDig, M_HUE_REF, lValue);
		break;

	case evisimsrcSaturation:
		assert(((lValue >= M_MIN_LEVEL) && (lValue <= M_MAX_LEVEL))
				|| (lValue == M_DEFAULT));
		MdigReference(m_milidDig, M_SATURATION_REF, lValue);
		break;

	case evisimsrcWhiteBalance:
		// LATER:  Specify the channel if not the default channel?
		assert(((lValue >= M_MIN_LEVEL) && (lValue <= M_MAX_LEVEL))
				|| (lValue == M_DEFAULT));
		MdigReference(m_milidDig, M_WHITE_REF, lValue);
		break;

	case evisimsrcColorEnable:
		// UNDONE:  Set m_fColorEnable?
		assert(0);
		hrRet = ResultFromScode(E_NOTIMPL);
		break;

	case evisimsrcNBuffers:
		if ((lValue > 0) && (lValue < 50))
		{
			m_pProvider->MeteorBufList().ReallocBufs(this, ((int) lValue)
					- m_pProvider->MeteorBufList().CBufInUse(this));

			if (m_pProvider->MeteorBufList().CBufCur(this) < (int) lValue)
				hrRet = ResultFromScode(E_FAIL);
		}
		else
		{
			hrRet = ResultFromScode(E_FAIL);
		}
		break;

#if M_MIL_USE_METEOR_II
	case evisimsrcMeteorfPreserveAspect:
		if (lValue ==0) {
			m_fPreserveAspect = FALSE;
			CheckScales();
			CheckFrameOrFieldGrab();
		} else if (lValue = 1) {
			m_fPreserveAspect = TRUE;
			CheckScales();
			CheckFrameOrFieldGrab();
		} else {
			hrRet = ResultFromScode(E_FAIL);
		}
		break;
#endif


	default:
		hrRet = ResultFromScode(E_FAIL);
		break;
	}

	return hrRet;
}

STDMETHODIMP CVisMeteor::IsPreferredImEncoding(THIS_ EVisImEncoding evisimencoding) CONST
{
	HRESULT hrRet;

	// What is the format of the 16-bit planar YUV encoding used by the Meteor?
	if ((evisimencoding == evisimencodingRGBA32)
			|| (evisimencoding == evisimencodingGray8bit)
			|| (evisimencoding == evisimencodingUYVY)  // UNDONE:  Is YUY2 the right encoding?
			|| (evisimencoding == evisimencodingYVU9)
			|| (evisimencoding == evisimencodingIYUV))
	{
		hrRet = ResultFromScode(S_OK);
	}
	else
	{
		hrRet = ResultFromScode(S_FALSE);
	}

	return hrRet;
}

STDMETHODIMP CVisMeteor::IsSupportedImEncoding(THIS_ EVisImEncoding evisimencoding) CONST
{
	return IsPreferredImEncoding(evisimencoding);
}


STDMETHODIMP CVisMeteor::HasDialog(THIS_ EVisVidDlg evisviddlg)
{
	return ResultFromScode(S_FALSE);
}

STDMETHODIMP_(LONG) CVisMeteor::DoDialog(THIS_ EVisVidDlg evisviddlg,
		HWND hwndParent)
{
	return ResultFromScode(E_FAIL);
}


// Attempt to specify a buffer to use to store image data.
STDMETHODIMP CVisMeteor::AddBuffer(THIS_ void *pvBuffer, unsigned long cbBuffer,
		VisImSrcMemCallback pfnDeleteBuffer, void *pvDeleteBuffer)
{
	// We can't specify the buffers used by the Meteor board, because they must be
	// DMA buffers.
	return ResultFromScode(E_FAIL);
}


// May wait until we get a new image if (imageMostRecent == imageCur).
// If the buffer was provided by the caller by using the AddBuffer call,
// the caller is responsible for freeing the buffer.
STDMETHODIMP CVisMeteor::GrabNext(THIS)
{
	HRESULT hrRet;

	EnterCriticalSection(&m_criticalsection);

#if 0
	// By removing this piece of code we get a frame rate speedup from
	// 20 fps to 30 fps.  It seems that some calls extra calls to MdigGrab
	// are needed, even though MdigGrab is called (possibly with
	// m_pMeteorBufNext) for every non-NULL value assigned to m_pMeteorBufCur.
	FILETIME filetime;
	GetSystemTimeAsFileTime(&filetime);

	if ((m_pMeteorBufCur != 0)
			&& ((filetime.dwLowDateTime - m_filetimeLastGrab.dwLowDateTime)
					< ec100nsFrameWaitMax))
	{
		hrRet = ResultFromScode(S_FALSE);
	}
	else
#endif
	{
		if (m_pMeteorBufCur == 0)
			m_pMeteorBufCur = m_pProvider->MeteorBufList().PBufGetPMeteor(this);

		// UNDONE:  Does this code need to be modified to use m_pMeteorBufNext if
		// m_pMeteorBufNext is nonzero?
		if (m_pMeteorBufCur != 0)
		{
			assert(m_pMeteorBufCur->Milid() != M_NULL);
			MdigGrab(m_milidDig, m_pMeteorBufCur->Milid());
			GetSystemTimeAsFileTime(&m_filetimeLastGrab);

			hrRet = ResultFromScode(S_OK);
		}
	}

	LeaveCriticalSection(&m_criticalsection);

	return hrRet;
}


bool CVisMeteor::FGetDigitizer(void)
{
	// Fail if we don't have a string identifying the desired digitizer.
	if (m_szDig == 0)
		return false;

	bool fmeteor_rgb;
	std::string strDeviceID = m_szDig;
	std::string::size_type ichNil = std::string::npos;
	if (strDeviceID.find("_RGB") != ichNil) {
		fmeteor_rgb = TRUE;
	} else {
		fmeteor_rgb = FALSE;
	}

	// Want to get lots of room for DMA buffers.  If there's a recommended size
	// given in the registry, use it.  If we can't get all the memory that we'd
	// like to get, try smaller sizes.  If there's no recommended size in the
	// registry, use M_DEFAULT to let the Meteor code use an appropriate buffer
	// size.
	// LATER:  Allow different DMAMem sizes for different Meteor devices by
	// adding registry keys under each device?
	int ckbDMABuf = 0;
	HKEY hkeyT;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, VIS_SZ_REG_IMSRC_DEVS "\\VisMeteor",
			0, KEY_QUERY_VALUE, &hkeyT) == ERROR_SUCCESS)
	{
		DWORD dwType;
		DWORD dwT = 0;
		DWORD cbT = sizeof(dwT);
		if ((RegQueryValueEx(hkeyT, "DMAMem", 0, &dwType,
						(BYTE *) &dwT, &cbT) == ERROR_SUCCESS)
				&& (dwType == REG_DWORD))
		{
			// Make sure that the registry value is a reasonably-sized
			// multiple of 256.
			if ((dwT & 0xff0000ff) == 0)
				ckbDMABuf = (int) dwT;
		}

		RegCloseKey(hkeyT);
	}

#if M_MIL_USE_METEOR_II
	void *pvSystemTypePtr = M_SYSTEM_METEOR_II;
#else // M_MIL_USE_METEOR_II
	void *pvSystemTypePtr = M_SYSTEM_METEOR;
#endif // M_MIL_USE_METEOR_II
	m_milidSys = M_NULL;
	while (ckbDMABuf > 0)
	{
		m_milidSys = MsysAlloc(pvSystemTypePtr,
				m_pProvider->LSysGetISys(m_iSys), M_DMA_BUF_SIZE((ckbDMABuf)), M_NULL);

		if (m_milidSys != M_NULL)
			break;

		ckbDMABuf -= 256;
	}
	
	// If no recommended size was given in the registry (or the above stuff
	// didn't work), try using M_DEFAULT when allocating the system.
	if (m_milidSys == M_NULL)
	{
		m_milidSys = MsysAlloc(pvSystemTypePtr,
				m_pProvider->LSysGetISys(m_iSys), M_DEFAULT, M_NULL);
	}

	// Fail if we can't allocate a system.
	if (m_milidSys == M_NULL)
		return false;

	// Once we have the system, try to get the digitizer.
	m_milidDig = MdigAlloc(m_milidSys, M_DEFAULT, m_szDig, M_DEFAULT, M_NULL);

	// Fail if we can't allocate a digitizer.
	if (m_milidDig == M_NULL)
		return false;

	// If needed, specify the channel to use.
	if ((m_milidDig != M_NULL) && (m_iChannel != 0))
	{
		long mpilChannel[5] = { M_DEFAULT, M_CH0, M_CH1, M_CH2, M_CH3 };

		MdigChannel(m_milidDig, mpilChannel[m_iChannel]);
	}

	// Also try to get the display control (so that we can use M_DISP
	// when we allocate buffers).
	m_milidDisp = MdispAlloc(m_milidSys, M_DEFAULT, M_DISPLAY_SETUP, M_DEFAULT, M_NULL);

	// Fail if we can't allocate a display controller.
	if (m_milidDisp == M_NULL)
		return false;

 	// Set-up the system.
   
	// Make sure 8 bits/pixel are delivered
    MsysControl(m_milidSys, M_ALLOC_BUF_RGB888_AS_RGB555, M_DISABLE);

    // LATER:  Set PCI latency and fast PCI to memory?
	// About 150 seems to give the best performance (?)
//	MsysControl(m_milidSys, M_PCI_LATENCY, 150);
	// I think this is the default
//	MsysControl(m_milidSys, M_FAST_PCI_TO_MEM, M_ENABLE);

	// Get the maximum size of captured images.
	m_sizeBaseline.cx = MdigInquire(m_milidDig, M_SIZE_X, M_NULL);
	m_sizeBaseline.cy = MdigInquire(m_milidDig, M_SIZE_Y, M_NULL);

	// Use half the maximum size of captured images for our default image size.
	if (!fmeteor_rgb) {	
		m_size.cx = m_sizeBaseline.cx / 2;
		m_size.cy = m_sizeBaseline.cy / 2;
	} else {
		m_size.cx = m_sizeBaseline.cx;
		m_size.cy = m_sizeBaseline.cy;
	}
	CheckScales();
	CheckFrameOrFieldGrab(true);

	// Things should work fine with either choice here, but asynchronous grabbing
	// may have advantages in some cases.
	// (We require asynchronous grabbing for the StartGrab callback.)
	MdigControl(m_milidDig, M_GRAB_MODE, M_ASYNCHRONOUS);

	if (!fmeteor_rgb)
		MdigControl(m_milidDig, M_GRAB_START_MODE, M_FIELD_START);

	MdigInquire(m_milidDig, M_FIELD_START_HANDLER_PTR,
			(void *) &m_pfnGrabStartHandler);
	MdigInquire(m_milidDig, M_FIELD_START_HANDLER_USER_PTR,
			(void *) &m_pvGrabStartHandler);
	MdigHookFunction(m_milidDig, M_FIELD_START, StaticGrabStartHandler, this);

	MdigInquire(m_milidDig, M_GRAB_END_HANDLER_PTR,
			(void *) &m_pfnGrabEndHandler);
	MdigInquire(m_milidDig, M_GRAB_END_HANDLER_USER_PTR,
			(void *) &m_pvGrabEndHandler);
	MdigHookFunction(m_milidDig, M_GRAB_END, StaticGrabEndHandler, this);

	// Find the default reference values.
	m_lRefBlackDefault =  MdigInquire(m_milidDig, M_DIG_REF_BLACK, M_NULL);
	m_lRefWhiteDefault =  MdigInquire(m_milidDig, M_DIG_REF_WHITE, M_NULL);
	if (!fmeteor_rgb) {
		m_lRefSharpnessDefault =  MdigInquire(m_milidDig, M_DIG_BRIGHTNESS_REF, M_NULL);
		m_lRefContrastDefault =  MdigInquire(m_milidDig, M_DIG_CONTRAST_REF, M_NULL);
		m_lRefHueDefault =  MdigInquire(m_milidDig, M_DIG_HUE_REF, M_NULL);
		m_lRefSaturationDefault =  MdigInquire(m_milidDig, M_DIG_SATURATION_REF, M_NULL);
	}


	// If we get here, we were able to allocate the digitizer.
	return true;
}


void CVisMeteor::ReadSettings(void)
{
	// UNDONE
}


// Set a callback function to be called when new frames arrive during
// a continuous grab operation.
STDMETHODIMP CVisMeteor::GetFrameCallback(T_PfnVisImSrcFrameCallback *ppfnCallback,
		void **ppvUser) CONST
{
	if (ppfnCallback != 0)
		*ppfnCallback = m_pfnCallback;
	if (ppvUser != 0)
		*ppvUser = m_pvUser;

	return ResultFromScode(S_OK);
}

STDMETHODIMP CVisMeteor::SetFrameCallback(T_PfnVisImSrcFrameCallback pfnCallback,
		void *pvUser)
{
	m_pfnCallback = pfnCallback;
	m_pvUser = pvUser;

	return ResultFromScode(S_OK);
}


unsigned __stdcall CVisMeteor::StaticThreadMain(void *pvMeteor)
{
	CVisMeteor *pcamera = (CVisMeteor *) pvMeteor;
	assert(pcamera != 0);
	return pcamera->ThreadMain();
}


unsigned __stdcall CVisMeteor::ThreadMain(void)
{
	// Message loop
	// UNDONE:  Probably don't need a message loop with the Meteor.
	MSG msg;
	while (::GetMessage(&msg, NULL, NULL, NULL) == 1)
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	return 0;
}


long MFTYPE CVisMeteor::StaticGrabStartHandler(long lHookType, MIL_ID milidEvent,
		void MPTYPE *pvMeteor)
{
	long lRet = M_NULL;
	CVisMeteor *pmeteor = (CVisMeteor *) pvMeteor;
	assert(pmeteor != 0);

	if (pmeteor != 0)
		lRet = pmeteor->GrabStartHandler(lHookType, milidEvent);

	return lRet;
}


long CVisMeteor::GrabStartHandler(long lHookType, MIL_ID milidEvent)
{
	long lRet = M_NULL;

	// If there is another hook function, call it.
	if (m_pfnGrabStartHandler != 0)
	{
		lRet = m_pfnGrabStartHandler(lHookType, milidEvent, m_pvGrabStartHandler);
	}

//	assert(m_pMeteorBufCur != 0);

	if (m_fInContinuousGrab)
	{
		FILETIME filetime;
		GetSystemTimeAsFileTime(&filetime);

		if ((m_pMeteorBufNext == 0)
				|| ((filetime.dwLowDateTime - m_filetimeLastGrab.dwLowDateTime)
					> ec100nsFrameWaitMax))
		{
			if (m_pMeteorBufNext == 0)
				m_pMeteorBufNext = m_pProvider->MeteorBufList().PBufGetPMeteor(this);

			if (m_pMeteorBufNext != 0)
			{
				assert(m_pMeteorBufNext->Milid() != M_NULL);
				MdigGrab(m_milidDig, m_pMeteorBufNext->Milid());
				GetSystemTimeAsFileTime(&m_filetimeLastGrab);
			}
		}
	}

	return lRet;
}


long MFTYPE CVisMeteor::StaticGrabEndHandler(long lHookType, MIL_ID milidEvent,
		void MPTYPE *pvMeteor)
{
	long lRet = M_NULL;
	CVisMeteor *pmeteor = (CVisMeteor *) pvMeteor;
	assert(pmeteor != 0);

	if (pmeteor != 0)
		lRet = pmeteor->GrabEndHandler(lHookType, milidEvent);

	return lRet;
}


long CVisMeteor::GrabEndHandler(long lHookType, MIL_ID milidEvent)
{
#ifdef _DEBUG
#ifdef SAVE
	LARGE_INTEGER liPerfStart;
	QueryPerformanceCounter(&liPerfStart);
#endif //  SAVE
#endif // _DEBUG

	long lRet = M_NULL;

	EnterCriticalSection(&m_criticalsection);

	// If there is another hook function, call it.
	if (m_pfnGrabEndHandler != 0)
	{
		lRet = m_pfnGrabEndHandler(lHookType, milidEvent, m_pvGrabEndHandler);
	}

	// UNDONE:  We'll want to be sure that the only Windows APIs called in
	// this function are PostMessage() and PostAppMessage().  We might need
	// to use another thread call back to the image sequence list.YUV16

	// UNDONE:  We really should use a critical section here and in the methods
	// that change m_pfnCallback to make sure that m_pfnCallback is not cleared
	// while we're processing this frame.

	// UNDONE:  This method takes too long.  Try passing the frame to the background
	// thread and having the background thread process the image.
	
	// It appears that m_pMeteorBufCur can be zero after frames are dropped.
	if (m_pMeteorBufCur != 0)
	{
		if (m_pfnCallback == 0)
		{
			CVisMeteorBufList::StaticReleaseBuf(m_pMeteorBufCur->PvData(),
					(void *) m_pMeteorBufCur);
			m_pMeteorBufCur = m_pMeteorBufNext;
			m_pMeteorBufNext = M_NULL;
		}
		else
		{
			SVisImSrcFrame imsrcframe;
			ZeroMemory(&imsrcframe, sizeof(imsrcframe));

			imsrcframe.m_rect.left = 0;
			imsrcframe.m_rect.top = 0;
			imsrcframe.m_rect.right = m_pMeteorBufCur->Size().cx;
			imsrcframe.m_rect.bottom = m_pMeteorBufCur->Size().cy;

			GetSystemTimeAsFileTime(&(imsrcframe.m_filetime));

			imsrcframe.m_pvData = m_pMeteorBufCur->PvData();
			imsrcframe.m_cbData = m_pMeteorBufCur->CbData();
			imsrcframe.m_evisimencoding = m_pMeteorBufCur->ImEncoding();

			// UNDONE:  add m_evisconvyuv.
			imsrcframe.m_evisconvyuv = evisconvyuvDefault;

			// For the beta, we don't support color maps.
			// LATER:  We might also try getting the color table from palette
			// entries by using the HPALETTE in the CAPSTATUS data structure.
			// Or it might be part of the BITMAPINFO structure.
			imsrcframe.m_prgbqColormap = 0;
			imsrcframe.m_crgbqColormap = 0;

			imsrcframe.dwReserved = 0;

			imsrcframe.m_pfnDeleteData = CVisMeteorBufList::StaticReleaseBuf;
			imsrcframe.m_pvDeleteData = (void *) m_pMeteorBufCur;

			imsrcframe.m_dwFlags = evisfiminfDMABuffer;

			// Clear our reference to the current buffer before making the callback.
			m_pMeteorBufCur = m_pMeteorBufNext;
			m_pMeteorBufNext = M_NULL;

			// We're now done with this buffer.  We can free it ourselves or the
			// frame callback function can free it.
			if (imsrcframe.m_rect.bottom > 0)
			{
	#ifdef _DEBUG
				// Remember the buffer information for asserts below.
				VisImSrcMemCallback pfnDeleteData = imsrcframe.m_pfnDeleteData;
				void *pvDeleteData = imsrcframe.m_pvDeleteData;
	#endif // _DEBUG

				// Tell the program that a new image is available.
				T_PfnVisImSrcFrameCallback pfnCallback = m_pfnCallback;
				if (pfnCallback != 0)
					pfnCallback((IVisImSrcDevice *) this, &imsrcframe, m_pvUser);

	#ifdef _DEBUG
				// If the program will free the buffer, imsrcframe.m_pfnDeleteData
				// and imsrcframe.m_pvDeleteData should both be set to zero.
				// Otherwise, we'll free the buffer below.
				assert((imsrcframe.m_pfnDeleteData == 0)
						|| (imsrcframe.m_pfnDeleteData == pfnDeleteData));
				assert((imsrcframe.m_pvDeleteData == 0)
						|| (imsrcframe.m_pvDeleteData == pvDeleteData));
				assert((imsrcframe.m_pfnDeleteData != 0)
						|| (imsrcframe.m_pvDeleteData == 0));
	#endif // _DEBUG
			}
			
			assert(M_NULL == 0);
			if (imsrcframe.m_pfnDeleteData != 0)
			{
				assert(imsrcframe.m_pvDeleteData != 0);
				imsrcframe.m_pfnDeleteData(imsrcframe.m_pvData, imsrcframe.m_pvDeleteData);
			}
		}
	}

	LeaveCriticalSection(&m_criticalsection);

#ifdef _DEBUG
#ifdef SAVE
	LARGE_INTEGER liPerfEnd;
	LARGE_INTEGER liPerfFreq;
	QueryPerformanceCounter(&liPerfEnd);
	QueryPerformanceFrequency(&liPerfFreq);
	LARGE_INTEGER lius;
	lius.QuadPart = (liPerfEnd.QuadPart - liPerfStart.QuadPart) * 1000000
			/ liPerfFreq.QuadPart;
	char szBuf[255];
	sprintf(szBuf, "Time to process frame = %d us.\n", lius.LowPart);
	OutputDebugString(szBuf);  // Around 8800us (but sometimes 12000us) in debug.
#endif //  SAVE
#endif // _DEBUG

	return lRet;
}
