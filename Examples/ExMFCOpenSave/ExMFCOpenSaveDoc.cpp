// ExMFCOpenSaveDoc.cpp : implementation of the CExMFCOpenSaveDoc class
//
// Copyright © 1997-2000 Microsoft Corporation, All Rights Reserved

#include "stdafx.h"
#include "VisCore.h"
#include "ExMFCOpenSave.h"

#include "ExMFCOpenSaveDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExMFCOpenSaveDoc

IMPLEMENT_DYNCREATE(CExMFCOpenSaveDoc, CDocument)

BEGIN_MESSAGE_MAP(CExMFCOpenSaveDoc, CDocument)
	//{{AFX_MSG_MAP(CExMFCOpenSaveDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExMFCOpenSaveDoc construction/destruction

CExMFCOpenSaveDoc::CExMFCOpenSaveDoc()
  : m_wCompression(0)  // Default compression
//	m_wCompression(100)  // Minimal compression (none with TGA)
{
	// Should we include alpha information when writing files?
	//CVisImageBase::SetIsAlphaWritten(false);
}

CExMFCOpenSaveDoc::~CExMFCOpenSaveDoc()
{
}

BOOL CExMFCOpenSaveDoc::OnNewDocument()
{
	// We don't want to have documents that are not associated with files.
	return FALSE;
}



/////////////////////////////////////////////////////////////////////////////
// CExMFCOpenSaveDoc serialization

void CExMFCOpenSaveDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// Saving is handled in CExMFCOpenSaveDoc::OnSaveDocument, so this
		// function shouldn't get called.
		assert(0);
	}
	else
	{
		// Loading is handled in CExMFCOpenSaveDoc::OnOpenDocument, so this
		// function shouldn't get called.
		assert(0);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CExMFCOpenSaveDoc diagnostics

#ifdef _DEBUG
void CExMFCOpenSaveDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CExMFCOpenSaveDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CExMFCOpenSaveDoc commands

void CExMFCOpenSaveDoc::DeleteContents() 
{
	// Invalidate the image.
	CVisRGBAByteImage imageT;
	m_image = imageT;
	
	CDocument::DeleteContents();
}

BOOL CExMFCOpenSaveDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	try
	{
		m_image.ReadFile(lpszPathName);
	}
	catch (...)  // "..." is bad - we could leak an exception object.
	{
		// Try using a grayscale image and then copying the pixels to
		// our color image.
		CVisByteImage image;

		try
		{
			image.ReadFile(lpszPathName);

			m_image.Allocate(image.Rect());

			image.CopyPixelsTo(m_image);
		}
		catch (...)  // "..." is bad - we could leak an exception object.
		{
			// Give up trying to read an image from this file.
			// Warn the user.
			AfxMessageBox("The file specified could not be opened.");

			return FALSE;
		}
	}
	
	return TRUE;
}

BOOL CExMFCOpenSaveDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	// We could also do this in CExMFCOpenSaveDoc::Serialize by calling
	// ar.GetFile()->GetFilePath() to get the file name.
	try
	{
		SVisFileDescriptor filedescriptorT;
		ZeroMemory(&filedescriptorT, sizeof(SVisFileDescriptor));
		filedescriptorT.has_alpha_channel = CVisImageBase::IsAlphaWritten();
		filedescriptorT.jpeg_quality = m_wCompression;

		filedescriptorT.filename = lpszPathName;

		m_image.WriteFile(filedescriptorT);
	}
	catch (...)  // "..." is bad - we could leak an exception object.
	{
		// Warn the user.
		AfxMessageBox("The file cound not be saved.");

		return FALSE;
	}
	
	return TRUE;
}
