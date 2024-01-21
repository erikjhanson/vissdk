// ExCameraView.h : interface of the CExCameraView class
//
// Copyright © 1997-2000 Microsoft Corporation, All Rights Reserved
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXCAMERAVIEW_H__C04069ED_02EC_11D1_AA81_0000F803FF4F__INCLUDED_)
#define AFX_EXCAMERAVIEW_H__C04069ED_02EC_11D1_AA81_0000F803FF4F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CExCameraView : public CView
{
protected: // create from serialization only
	CExCameraView();
	DECLARE_DYNCREATE(CExCameraView)

// Attributes
public:
	CExCameraDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExCameraView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CExCameraView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:


// Generated message map functions
protected:
	//{{AFX_MSG(CExCameraView)
	afx_msg void OnDoCompressionDialog();
	afx_msg void OnUpdateDoCompressionDialog(CCmdUI* pCmdUI);
	afx_msg void OnDoDisplayDialog();
	afx_msg void OnUpdateDoDisplayDialog(CCmdUI* pCmdUI);
	afx_msg void OnDoFormatDialog();
	afx_msg void OnUpdateDoFormatDialog(CCmdUI* pCmdUI);
	afx_msg void OnDoSourceDialog();
	afx_msg void OnUpdateDoSourceDialog(CCmdUI* pCmdUI);
	afx_msg void OnSaveImage();
	afx_msg void OnUpdateSaveImage(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ExCameraView.cpp
inline CExCameraDoc* CExCameraView::GetDocument()
   { return (CExCameraDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXCAMERAVIEW_H__C04069ED_02EC_11D1_AA81_0000F803FF4F__INCLUDED_)
