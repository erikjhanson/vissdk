// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
// Copyright © 1997-2000 Microsoft Corporation, All Rights Reserved

#if !defined(AFX_STDAFX_H__6B8A267F_D2C6_11D0_AA63_0000F803FF4F__INCLUDED_)
#define AFX_STDAFX_H__6B8A267F_D2C6_11D0_AA63_0000F803FF4F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC OLE automation classes
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


// We could replace "stdafx.h" with "VisCore.h" in all project files.
// We chose to include "VisCore.h" in "stdafx.h" to minimize changes
// to the project.
#include "VisWin.h"
#if VIS_OS_IS_NT
	#include "VisCore.h"
#endif // VIS_OS_IS_NT

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.


#endif // !defined(AFX_STDAFX_H__6B8A267F_D2C6_11D0_AA63_0000F803FF4F__INCLUDED_)
