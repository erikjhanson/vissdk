// ExMFCOpenSave.h : main header file for the EXMFCOPENSAVE application
//
// Copyright © 1997-2000 Microsoft Corporation, All Rights Reserved

#if !defined(AFX_EXMFCOPENSAVE_H__BC7985B5_CE09_11D0_AA63_0000F803FF4F__INCLUDED_)
#define AFX_EXMFCOPENSAVE_H__BC7985B5_CE09_11D0_AA63_0000F803FF4F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CExMFCOpenSaveApp:
// See ExMFCOpenSave.cpp for the implementation of this class
//

class CExMFCOpenSaveApp : public CWinApp
{
public:
	CExMFCOpenSaveApp();

	// Should printed images be scaled?
	bool FScaleWhenPrinting(void) const
		{ return (m_wPercentPrintPage > 0); }

	// How much should printed images be scaled?
	int PercentPrintPage(void) const
		{ return m_wPercentPrintPage; }


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExMFCOpenSaveApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CExMFCOpenSaveApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFullScreenPrint();
	afx_msg void OnUpdateFullScreenPrint(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	// Size of printed images as a percentage of page size.
	// (If this is 0, no scaling should be done when printing images.)
	int m_wPercentPrintPage;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXMFCOPENSAVE_H__BC7985B5_CE09_11D0_AA63_0000F803FF4F__INCLUDED_)
