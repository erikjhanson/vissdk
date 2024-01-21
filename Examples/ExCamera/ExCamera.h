// ExCamera.h : main header file for the EXCAMERA application
//
// Copyright © 1997-2000 Microsoft Corporation, All Rights Reserved

#if !defined(AFX_EXCAMERA_H__C04069E5_02EC_11D1_AA81_0000F803FF4F__INCLUDED_)
#define AFX_EXCAMERA_H__C04069E5_02EC_11D1_AA81_0000F803FF4F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CExCameraApp:
// See ExCamera.cpp for the implementation of this class
//

class CExCameraApp : public CWinApp
{
public:
	CExCameraApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExCameraApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CExCameraApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXCAMERA_H__C04069E5_02EC_11D1_AA81_0000F803FF4F__INCLUDED_)
