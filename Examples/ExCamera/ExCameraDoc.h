// ExCameraDoc.h : interface of the CExCameraDoc class
//
// Copyright © 1997-2000 Microsoft Corporation, All Rights Reserved
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXCAMERADOC_H__C04069EB_02EC_11D1_AA81_0000F803FF4F__INCLUDED_)
#define AFX_EXCAMERADOC_H__C04069EB_02EC_11D1_AA81_0000F803FF4F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


//typedef BYTE							CExCameraPixel;
//typedef unsigned short				CExCameraPixel;  // Should fail
//typedef unsigned long					CExCameraPixel;
typedef CVisRGBABytePixel				CExCameraPixel;
//typedef CVisYUVABytePixel				CExCameraPixel;
//typedef CVisYUVAUShortPixel			CExCameraPixel;  // Won't display


typedef CVisImage<CExCameraPixel>		CExCameraImage;


class CExCameraDoc : public CDocument
{
protected: // create from serialization only
	CExCameraDoc();
	DECLARE_DYNCREATE(CExCameraDoc)

// Attributes
public:
	CVisSequence<CExCameraPixel> m_sequence;

	// Note:  We have this member so that the background thread can invalidate
	// the current view.  The background thread can't call functions like
	// UpdateAllViews because MFC is not multi-threaded.
	HWND m_hwndView;

	CExCameraImage Image(void)
	{
		return m_image;
	}

	void ImageDisplayed(void);

	void DialogDisplayed(void);

	void GrabNext(DWORD cmsTimeout = 2000);

	bool FInBackgroundUpdate(void)
	{
		return m_fUpdateInBackground;
	}

	void SetBackgroundUpdate(bool f = true)
	{
		if (f)
			StartBackgroundUpdate();
		else
			StopBackgroundUpdate();
	}

	void StartBackgroundUpdate(void);

	void StopBackgroundUpdate(void);

	inline void ViewAttached(HWND hwnd)
	{
		SetBackgroundUpdate(m_sequence.HasImageSource() &&
				m_sequence.ImageSource().IsValid());
		m_hwndView = hwnd;
		m_pThread->ResumeThread();
	}


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExCameraDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void DeleteContents();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CExCameraDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CExCameraImage m_image;
	CWinThread *m_pThread;
	bool m_fKillThread;
	DWORD m_cmsStart;
	DWORD m_cmsEnd;
	DWORD m_cFrameGrabbed;
	DWORD m_cFrameDisplayed;
	bool m_fUpdateInBackground;
	bool m_fGrabSingleFrame;
	bool m_fUpdateDisplay;

	CVisTimer2 m_timerGrabbed;
	CVisTimer2 m_timerDisplayed;
	double m_dblRateGrabbed;
	double m_dblRateDisplayed;

	static UINT __cdecl StaticThreadMain(void *pvDocument);
	UINT ThreadMain(void);

	void ResetStatusInfo(void);

	void UpdateStatusInfo(void);

// Generated message map functions
protected:
	//{{AFX_MSG(CExCameraDoc)
	afx_msg void OnDecSize();
	afx_msg void OnIncSize();
	afx_msg void OnUpdateDecSize(CCmdUI* pCmdUI);
	afx_msg void OnUpdateIncSize(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDisplay();
	afx_msg void OnUpdateUpdateDisplay(CCmdUI* pCmdUI);
	afx_msg void OnCapture();
	afx_msg void OnUpdateCapture(CCmdUI* pCmdUI);
	afx_msg void OnGrab();
	afx_msg void OnUpdateGrab(CCmdUI* pCmdUI);
	afx_msg void OnSelectDevice();
	afx_msg void OnUpdateSelectDevice(CCmdUI* pCmdUI);
	afx_msg void OnGrabOne();
	afx_msg void OnUpdateGrabOne(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXCAMERADOC_H__C04069EB_02EC_11D1_AA81_0000F803FF4F__INCLUDED_)
