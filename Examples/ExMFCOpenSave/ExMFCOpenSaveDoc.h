// ExMFCOpenSaveDoc.h : interface of the CExMFCOpenSaveDoc class
//
// Copyright © 1997-2000 Microsoft Corporation, All Rights Reserved
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXMFCOPENSAVEDOC_H__BC7985BD_CE09_11D0_AA63_0000F803FF4F__INCLUDED_)
#define AFX_EXMFCOPENSAVEDOC_H__BC7985BD_CE09_11D0_AA63_0000F803FF4F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CExMFCOpenSaveDoc : public CDocument
{
protected: // create from serialization only
	CExMFCOpenSaveDoc();
	DECLARE_DYNCREATE(CExMFCOpenSaveDoc)

// Attributes
public:
	// Get the image.
	const CVisImageBase& Image() const
		{ return m_image; }

	CVisImageBase& Image()
		{ return m_image; }

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExMFCOpenSaveDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void DeleteContents();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CExMFCOpenSaveDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	// The image.
	CVisRGBAByteImage m_image;
	int m_wCompression;

// Generated message map functions
protected:
	//{{AFX_MSG(CExMFCOpenSaveDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXMFCOPENSAVEDOC_H__BC7985BD_CE09_11D0_AA63_0000F803FF4F__INCLUDED_)
