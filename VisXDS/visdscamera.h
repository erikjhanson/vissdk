////////////////////////////////////////////////////////////////////////////
//
// @doc INTERNAL EXTERNAL VISDSCAMERA 
//
// @module VisDSCamera.h |
//
// This file defines the <c CVisDSCamera> class.  It is derived from
// the <c IVisImSrcSettings> and <c IVisImSrcDevice> interfaces.
// It is used to work with (adjust settings for and get images from)
// a DS camera.
//
// <nl>
//
// Copyright © 1997-2000 Microsoft Corporation, All Rights Reserved
// <nl>
//
// @xref <l VisDSCamera\.cpp>
//
////////////////////////////////////////////////////////////////////////////


#ifndef __VIS_IMSRC_VISDSCAMERA_H__
#define __VIS_IMSRC_VISDSCAMERA_H__


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000



// We don't want any dependencies on the CVisImage class, so we don't
// include VisCore.h in this file.
#include <Windows.h>
#include <amstream.h>
#include <amvideo.h>
#include <ole2.h>

#include <assert.h>

#include <string>
#include <streams.h>
#include <mmreg.h>

#include <itrnsfer.h>
#include <renduids.h>
#include "..\VisImSrc\VisImSrcNoMFC.h"
#include "..\VisCore\VisCritSect.h"

#include "VisDSCamDLL.h"
#include "VisDSProvider.h"


class CVisDSCamera : public IVisImSrcSettings,
		public IVisImSrcDevice2
{
public:
	//------------------------------------------------------------------
	// Constructor and destructor

    CVisDSCamera(void);
	virtual ~CVisDSCamera(void);

	// Returns S_OK if successful.
	HRESULT CVisDSCamera::Init(IEnumMoniker *pEm, int iCaptureDevice);


	//------------------------------------------------------------------
	// IUnknown methods

    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppvObject);
    STDMETHOD_(ULONG, AddRef) (THIS);
    STDMETHOD_(ULONG, Release) (THIS);


	//------------------------------------------------------------------
	// IVisImageSourceSettings properties

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
	// IVisImSrcDevice methods

	STDMETHOD(IsPreferredImEncoding) (THIS_ EVisImEncoding evisimencoding) CONST;
	STDMETHOD(IsSupportedImEncoding) (THIS_ EVisImEncoding evisimencoding) CONST;
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
			HWND hwndParent);
    STDMETHOD(GetSzID) (THIS_ LPSTR sz, LONG *pcb);
    STDMETHOD(GetSzName) (THIS_ LPSTR sz, LONG *pcb);


	//------------------------------------------------------------------
	// IVisImSrcDevice2 methods

	STDMETHOD(HasPropertyPage) (THIS_ EVisDSPropPage evisdsproppage);
    STDMETHOD_(LONG, DoPropertyPage) (THIS_ EVisDSPropPage evisdsproppage,
			HWND hwndParent);


private:
	enum { ecchProviderSep = 3 };

	enum { eidwFourCCUnknownLim = 10 };

	enum EFlip
	{
		eflipUnknown = - 1,
		eflipNo,
		eflipYes
	};

	unsigned long m_cRef;
	std::string m_strID;
	ICaptureGraphBuilder *pBuilder;
	IGraphBuilder *pFg;
	IBaseFilter *pVCap;
	IBaseFilter *pVTrns;
	ITransfer *pTransfer;
	IMediaControl *pMC;
	IAMStreamConfig * pVSC;
	IAMCameraControl * pCC;
	IAMVideoProcAmp * pVPA;
	IAMVfwCaptureDialogs *pDlg;
	EVisImEncoding m_evisimencoding;
	EVisConvYUV m_evisconvyuv;
	EVisImEncoding m_evisimencodingGrabCur;
	T_PfnVisImSrcFrameCallback m_pfnCallback;
	void *m_pvUser;
	DWORD m_rgdwFourCCUnknown[eidwFourCCUnknownLim];
	int m_rgcbitFourCCUnknown[eidwFourCCUnknownLim];
	int m_cdwFourCCUnknownCur;
	DWORD m_rgdwFourCCImEncoding[evisimencodingLim];
	EFlip m_rgeflipImEncoding[evisimencodingLim];
	bool m_rgfHaveImEncoding[evisimencodingLim];
	bool m_fInContinuousGrab;
	bool m_fInSingleFrameGrab;
	bool m_fFlippedImage;
	bool m_fUseNegHtToFlip;
	bool m_fCanSetVideoFormat;
	long m_ImageWidth;
	long m_ImageHeight;
	HANDLE m_SingleFrameSemaphore;

	static LRESULT CALLBACK StaticFrameCallbackProc(CVisDSCamera * pDS,
		IMediaSample * pMS);
	LRESULT FrameCallbackProc(IMediaSample * pMS);

	const char *SzName(void) const;
	void SetupCaptureParams(void);
	bool FReadSettings(void);
	BITMAPINFOHEADER *GetBmihFromMT(AM_MEDIA_TYPE * pmt);
	AM_MEDIA_TYPE *PmtGet(void) const;
	void FreePmt(AM_MEDIA_TYPE * pmt) const;
	EVisImEncoding EVisImEncodingGet(BITMAPINFOHEADER *pbmih = 0,
			bool fUseCachedInfo = true);
	void UpdateCachedInfo(void);
	bool FFindImEncodings(void);
	EVisImEncoding ImEncodingGetDwFourCC(DWORD dwFourCC, int cbit);
	void FoundUnknownDwFourCC(DWORD dwFourCC, int cbit);
	DWORD DwFourCCGetImEncoding(EVisImEncoding evisimencoding) const;
    BOOL MakeBuilder(void);
	BOOL MakeGraph(void);
	HRESULT ConnectFilters();
	HRESULT DisconnectFilters();
};


#endif // __VIS_IMSRC_VISDSCAMERA_H__
