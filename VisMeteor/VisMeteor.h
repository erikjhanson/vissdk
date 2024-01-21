// VisMeteor.h
//
// Copyright © 1997-2000 Microsoft Corporation, All Rights Reserved

// UNDONE:  Need to expose another method to enumerate all
// available devices.

#pragma once

// We don't want any dependencies on the CVisImage class, so we don't
// include VisCore.h in this file.
#include <Windows.h>
#include <ole2.h>

#include <assert.h>

#include <string>

#include <mil.h>

#include "..\VisImSrc\VisImSrcNoMFC.h"

#include "VisMetBufList.h"

#include "VisMeteorDLL.h"
#include "VisMeteorProvider.h"


#if M_MIL_USE_METEOR
#pragma comment(lib, "milmet")
#endif // M_MIL_USE_METEOR


#if M_MIL_USE_METEOR_II
#pragma comment(lib, "milmet2")
#endif // M_MIL_USE_METEOR_II



class CVisMeteor : public IVisImSrcSettings, public IVisImSrcDevice
{
public:
	//------------------------------------------------------------------
	// @group Create function and destructor

    CVisMeteor(class CVisMeteorProvider *pProvider, int iDevice,
			int iSys, const char *szDig, int iChannel);
	virtual ~CVisMeteor(void);

	// Returns S_OK if successful.
	HRESULT CVisMeteor::Init(void);


	//------------------------------------------------------------------
	// @group IUnknown methods

    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppvObject);
    STDMETHOD_(ULONG, AddRef) (THIS);
    STDMETHOD_(ULONG, Release) (THIS);


	//------------------------------------------------------------------
	// @group IVisImageSourceSettings properties

	// Properties of this image source.
	STDMETHOD(GetImSrcRange) (THIS_ EVisImSrc evisimsrc, LONG *plMin,
			LONG *plMax, LONG *pdlStepping VISDEFAULT(0),
			LONG *plDefault VISDEFAULT(0), LONG *plFlags VISDEFAULT(0))
			CONST;
	STDMETHOD(GetImSrcValue) (THIS_ EVisImSrc evisimsrc, LONG *plValue)
			CONST;
	STDMETHOD(SetImSrcValue) (THIS_ EVisImSrc evisimsrc, LONG lValue,
			LONG lFlags VISDEFAULT(evisimsrcflagAuto));
	STDMETHOD(SaveSettings) (THIS_ bool fForAllApps VISDEFAULT(false));


	//------------------------------------------------------------------
	// @group IVisImSrcDevice methods

	STDMETHOD(IsPreferredImEncoding) (THIS_ EVisImEncoding evisimencoding)
			CONST;
	STDMETHOD(IsSupportedImEncoding) (THIS_ EVisImEncoding evisimencoding)
			CONST;
    STDMETHOD(GrabNext) (THIS);
	STDMETHOD(AddBuffer) (THIS_ void *pvBuffer, ULONG cbBuffer,
			VisImSrcMemCallback pfnDeleteBuffer, void *pvDeleteBuffer);
	STDMETHOD(GetFrameCallback) (THIS_
			T_PfnVisImSrcFrameCallback *ppfnCallback, void **ppvUser)
			CONST;
	STDMETHOD(SetFrameCallback) (THIS_
			T_PfnVisImSrcFrameCallback pfnCallback, void *pvUser);
    STDMETHOD(HasDialog) (THIS_ EVisVidDlg evisviddlg);
    STDMETHOD_(LONG, DoDialog) (THIS_ EVisVidDlg evisviddlg,
			HWND hwndParent VISDEFAULT(0));
    STDMETHOD(GetSzID) (THIS_ LPSTR sz, LONG *pcb);
    STDMETHOD(GetSzName) (THIS_ LPSTR sz, LONG *pcb);


	//------------------------------------------------------------------
	// @group Meteor properties


	inline int IDevice(void) const
		{ return m_iDevice; }

	inline int ISys(void) const
		{ return m_iSys; }

	inline char *SzDig(void) const
		{ return m_szDig; }

	inline int IChannel(void) const
		{ return m_iChannel; }

	inline SIZE SizeCur(void) const
		{ return m_size; }

	inline SIZE SizeBaseline(void) const
		{ return m_sizeBaseline; }

	inline MIL_ID MilidSys(void) const
		{ return m_milidSys; }

	inline MIL_ID MilidDig(void) const
		{ return m_milidDig; }

	inline MIL_ID MilidDisp(void) const
		{ return m_milidDisp; }

	inline EVisImEncoding ImEncoding(void) const
		{ return m_evisimencoding; }


private:
	enum { ecchProviderSep = 3 };
	enum { ec100nsFrameWaitMax = 5000000 };
	
	unsigned long m_cRef;
	class CVisMeteorProvider *m_pProvider;
	int m_iDevice;
	int m_iSys;
	char *m_szDig;
	int m_iChannel;
	SIZE m_size;
	SIZE m_sizeBaseline;
	MIL_ID m_milidSys;
	MIL_ID m_milidDig;
	MIL_ID m_milidDisp;
	CVisMeteorBuf *m_pMeteorBufCur;
	CVisMeteorBuf *m_pMeteorBufNext;
	MDIGHOOKFCTPTR m_pfnGrabStartHandler;
	void MPTYPE *m_pvGrabStartHandler;
	MDIGHOOKFCTPTR m_pfnGrabEndHandler;
	void MPTYPE *m_pvGrabEndHandler;
	std::string m_strID;
	T_PfnVisImSrcFrameCallback m_pfnCallback;
	void *m_pvUser;
	bool m_fInContinuousGrab;
	bool m_fInSingleFrameGrab;
	EVisImEncoding m_evisimencoding;
	bool m_fColorEnable;
	bool m_fPreserveAspect;
	LONG m_lRefBlackDefault;
	LONG m_lRefSharpnessDefault;
	LONG m_lRefContrastDefault;
	LONG m_lRefHueDefault;
	LONG m_lRefSaturationDefault;
	LONG m_lRefWhiteDefault;
	double m_dblScale,m_dblScaleX, m_dblScaleY;
	bool m_fGrabFields;
	FILETIME m_filetimeLastGrab;

	HANDLE m_hThread;
	DWORD m_dwThreadID;
	CRITICAL_SECTION m_criticalsection;

	static unsigned __stdcall StaticThreadMain(void *pvMeteor);
	unsigned __stdcall ThreadMain(void);

	static long MFTYPE StaticGrabStartHandler(long lHookType, MIL_ID milidEvent,
			void MPTYPE *pvMeteor);
	long GrabStartHandler(long lHookType, MIL_ID milidEvent);

	static long MFTYPE StaticGrabEndHandler(long lHookType, MIL_ID milidEvent,
			void MPTYPE *pvMeteor);
	long GrabEndHandler(long lHookType, MIL_ID milidEvent);

	bool FGetDigitizer(void);
	void ReadSettings(void);

	bool CheckScales(void);
	void CheckFrameOrFieldGrab(bool fInit = false);
};


#include "VisMeteor.inl"
