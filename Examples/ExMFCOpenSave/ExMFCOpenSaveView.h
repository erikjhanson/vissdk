// ExMFCOpenSaveView.h : interface of the CExMFCOpenSaveView class
//
// Copyright © 1997-2000 Microsoft Corporation, All Rights Reserved
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXMFCOPENSAVEVIEW_H__BC7985BF_CE09_11D0_AA63_0000F803FF4F__INCLUDED_)
#define AFX_EXMFCOPENSAVEVIEW_H__BC7985BF_CE09_11D0_AA63_0000F803FF4F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CExMFCOpenSaveView : public CScrollView
{
protected: // create from serialization only
	CExMFCOpenSaveView();
	DECLARE_DYNCREATE(CExMFCOpenSaveView)

// Attributes
public:
	CExMFCOpenSaveDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExMFCOpenSaveView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CExMFCOpenSaveView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CExMFCOpenSaveView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ExMFCOpenSaveView.cpp
inline CExMFCOpenSaveDoc* CExMFCOpenSaveView::GetDocument()
   { return (CExMFCOpenSaveDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXMFCOPENSAVEVIEW_H__BC7985BF_CE09_11D0_AA63_0000F803FF4F__INCLUDED_)
