// MainFrm.h : interface of the CMainFrame class
//
// Copyright © 1997-2000 Microsoft Corporation, All Rights Reserved
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__C04069E9_02EC_11D1_AA81_0000F803FF4F__INCLUDED_)
#define AFX_MAINFRM_H__C04069E9_02EC_11D1_AA81_0000F803FF4F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CMainFrame : public CFrameWnd
{
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:
	void ShowCFrameCms(DWORD cFrameGrabbed, DWORD cFrameDisplayed, DWORD cms)
	{
		char szGrabbed[256];
		char szDisplayed[256];

		if (cms == 0)
		{
			*szGrabbed = 0;
			*szDisplayed = 0;
		}
		else
		{
			DWORD cFrameGrabbedIn100s = 100000 * cFrameGrabbed / cms;
			sprintf(szGrabbed, "%d.%02d fps",
					cFrameGrabbedIn100s / 100,
					cFrameGrabbedIn100s % 100);

			DWORD cFrameDisplayedIn100s = 100000 * cFrameDisplayed / cms;
			sprintf(szDisplayed, "%d.%02d fps", 
					cFrameDisplayedIn100s / 100, 
					cFrameDisplayedIn100s % 100);
		}

		m_wndStatusBar.SetPaneText(eipaneFramesGrabbed, szGrabbed, true);
		m_wndStatusBar.SetPaneText(eipaneFramesDisplayed, szDisplayed, true);
	}

	void ShowFrameRates(double dblRateGrabbed, double dblRateDisplayed)
	{
		char szT[256];

		sprintf(szT, "%4.2lf fps", dblRateGrabbed);
		m_wndStatusBar.SetPaneText(eipaneFramesGrabbed, szT, false);

		sprintf(szT, "%4.2lf fps", dblRateDisplayed);
		m_wndStatusBar.SetPaneText(eipaneFramesDisplayed, szT, true);
	}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

	enum
	{
		eipaneFramesGrabbed = 1,
		eipaneFramesDisplayed
	};


// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__C04069E9_02EC_11D1_AA81_0000F803FF4F__INCLUDED_)
