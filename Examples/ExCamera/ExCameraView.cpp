// ExCameraView.cpp : implementation of the CExCameraView class
//
// Copyright © 1997-2000 Microsoft Corporation, All Rights Reserved

#include "stdafx.h"
#include "VisImSrc.h"
#include "ExCamera.h"

#include <math.h>

#include "VisImSrc.h"

#include "ExCameraDoc.h"
#include "ExCameraView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExCameraView

IMPLEMENT_DYNCREATE(CExCameraView, CView)

BEGIN_MESSAGE_MAP(CExCameraView, CView)
	//{{AFX_MSG_MAP(CExCameraView)
	ON_COMMAND(ID_DO_COMPRESSION_DIALOG, OnDoCompressionDialog)
	ON_UPDATE_COMMAND_UI(ID_DO_COMPRESSION_DIALOG, OnUpdateDoCompressionDialog)
	ON_COMMAND(ID_DO_DISPLAY_DIALOG, OnDoDisplayDialog)
	ON_UPDATE_COMMAND_UI(ID_DO_DISPLAY_DIALOG, OnUpdateDoDisplayDialog)
	ON_COMMAND(ID_DO_FORMAT_DIALOG, OnDoFormatDialog)
	ON_UPDATE_COMMAND_UI(ID_DO_FORMAT_DIALOG, OnUpdateDoFormatDialog)
	ON_COMMAND(ID_DO_SOURCE_DIALOG, OnDoSourceDialog)
	ON_UPDATE_COMMAND_UI(ID_DO_SOURCE_DIALOG, OnUpdateDoSourceDialog)
	ON_COMMAND(ID_SAVE_IMAGE, OnSaveImage)
	ON_UPDATE_COMMAND_UI(ID_SAVE_IMAGE, OnUpdateSaveImage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExCameraView construction/destruction

CExCameraView::CExCameraView()
{
	// TODO: add construction code here

}

CExCameraView::~CExCameraView()
{
}

BOOL CExCameraView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CExCameraView drawing

void CExCameraView::OnDraw(CDC* pDC)
{
	CExCameraDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CExCameraImage image = pDoc->Image();
	if (image.IsValid())
	{
		image.DisplayInHdc(pDC->m_hDC);
	}
	else
	{
		// Display a test pattern.
		unsigned long *rgul = new unsigned long[640 * 480];
		for (int i = 0; i < 640 * 480; ++i)
		{
			// (Note that red and blue bytes in pixels are different
			// than the red and blue bytes in COLORREFs, so the
			// following statment would make a red box on the screen:
			// rgul[i] = (unsigned long) RGB(0, 0, 0xff); )
			rgul[i] = 0xff << 8 * ((i % 640) / 160);
		}

		int dx = 640;
		int dy = 320;
		BITMAPINFOHEADER bitmapinfoheader;
		bitmapinfoheader.biSize = sizeof(BITMAPINFOHEADER);
		bitmapinfoheader.biWidth = 640;
		bitmapinfoheader.biHeight = - 480;
		bitmapinfoheader.biPlanes = 1;
		bitmapinfoheader.biBitCount = 32;
		bitmapinfoheader.biCompression = BI_RGB;
		bitmapinfoheader.biSizeImage = 0;
		bitmapinfoheader.biXPelsPerMeter = 2700;
		bitmapinfoheader.biYPelsPerMeter = 2700;
		bitmapinfoheader.biClrUsed = 0;
		bitmapinfoheader.biClrImportant = 0;

		SetDIBitsToDevice(pDC->m_hDC, 0, 0, dx, dy, 0, 0, 0,
				dy, rgul, (BITMAPINFO *) &bitmapinfoheader,
				DIB_RGB_COLORS);

		delete[] rgul;
	}

	// Tell the document that a new image was displayed.
	// UNDONE:  We should check timestamps an only call this when the
	// timestamp of this image does not match the timestamp of the previous
	// image.
	pDoc->ImageDisplayed();
}

/////////////////////////////////////////////////////////////////////////////
// CExCameraView diagnostics

#ifdef _DEBUG
void CExCameraView::AssertValid() const
{
	CView::AssertValid();
}

void CExCameraView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CExCameraDoc* CExCameraView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CExCameraDoc)));
	return (CExCameraDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CExCameraView message handlers

void CExCameraView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
	CExCameraDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->ViewAttached(m_hWnd);
}

void CExCameraView::OnDoCompressionDialog() 
{
	CExCameraDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	assert(pDoc->m_sequence.ImageSource().IsValid());
	pDoc->m_sequence.ImageSource().DoDialog(evisviddlgVFWCompression, m_hWnd);

	// Tell the document that a dialog was displayed (so that it knows
	// that the settings may have changed).
	pDoc->DialogDisplayed();
}

void CExCameraView::OnUpdateDoCompressionDialog(CCmdUI* pCmdUI) 
{
	CExCameraDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	BOOL fHasDialog = (pDoc->m_sequence.HasImageSource())
			&& (pDoc->m_sequence.ImageSource().IsValid())
			&& (pDoc->m_sequence.ImageSource().HasDialog(
					evisviddlgVFWCompression))
			&& (!pDoc->m_sequence.ImageSource().UseContinuousGrab());

	pCmdUI->Enable(fHasDialog);
}

void CExCameraView::OnDoDisplayDialog() 
{
	CExCameraDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	assert(pDoc->m_sequence.ImageSource().IsValid());
	pDoc->m_sequence.ImageSource().DoDialog(evisviddlgVFWDisplay, m_hWnd);

	// Tell the document that a dialog was displayed (so that it knows
	// that the settings may have changed).
	pDoc->DialogDisplayed();
}

void CExCameraView::OnUpdateDoDisplayDialog(CCmdUI* pCmdUI) 
{
	CExCameraDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	BOOL fHasDialog = (pDoc->m_sequence.HasImageSource())
			&& (pDoc->m_sequence.ImageSource().IsValid())
			&& (pDoc->m_sequence.ImageSource().HasDialog(
					evisviddlgVFWDisplay))
			&& (!pDoc->m_sequence.ImageSource().UseContinuousGrab());

	pCmdUI->Enable(fHasDialog);
}

void CExCameraView::OnDoFormatDialog() 
{
	CExCameraDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	assert(pDoc->m_sequence.ImageSource().IsValid());
	pDoc->m_sequence.ImageSource().DoDialog(evisviddlgVFWFormat, m_hWnd);

	// Tell the document that a dialog was displayed (so that it knows
	// that the settings may have changed).
	pDoc->DialogDisplayed();
}

void CExCameraView::OnUpdateDoFormatDialog(CCmdUI* pCmdUI) 
{
	CExCameraDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	BOOL fHasDialog = (pDoc->m_sequence.HasImageSource())
			&& (pDoc->m_sequence.ImageSource().IsValid())
			&& (pDoc->m_sequence.ImageSource().HasDialog(
					evisviddlgVFWFormat))
			&& (!pDoc->m_sequence.ImageSource().UseContinuousGrab());

	pCmdUI->Enable(fHasDialog);
}

void CExCameraView::OnDoSourceDialog() 
{
	CExCameraDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	assert(pDoc->m_sequence.ImageSource().IsValid());
	pDoc->m_sequence.ImageSource().DoDialog(evisviddlgVFWSource, m_hWnd);

	// Tell the document that a dialog was displayed (so that it knows
	// that the settings may have changed).
	pDoc->DialogDisplayed();
}

void CExCameraView::OnUpdateDoSourceDialog(CCmdUI* pCmdUI) 
{
	CExCameraDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	BOOL fHasDialog = (pDoc->m_sequence.HasImageSource())
			&& (pDoc->m_sequence.ImageSource().IsValid())
			&& (pDoc->m_sequence.ImageSource().HasDialog(
					evisviddlgVFWSource))
			&& (!pDoc->m_sequence.ImageSource().UseContinuousGrab());

	pCmdUI->Enable(fHasDialog);
}

void CExCameraView::OnSaveImage() 
{
	CExCameraDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CExCameraImage image = pDoc->Image();

	if (image.IsValid())
	{
		assert(image.Width() > 0);
		assert(image.Height() > 0);

		static int iFileName = 0;
		const char *szNameFormat = "ExCam%03d.bmp";
		char szName[100];
		int cTry = 0;
		
		sprintf(szName, szNameFormat, iFileName);
		WIN32_FIND_DATA win32finddata;
		HANDLE hFindFirstFile = FindFirstFile(szName, &win32finddata);
		while ((hFindFirstFile != INVALID_HANDLE_VALUE) && (cTry < 1000))
		{
			FindClose(hFindFirstFile);
			
			if (++iFileName == 1000)
				iFileName = 0;
			++ cTry;
			
			sprintf(szName, szNameFormat, iFileName);
			hFindFirstFile = FindFirstFile(szName, &win32finddata);
		}

		if (cTry < 1000)
		{
			image.WriteFile(szName);
		}
	}
}

void CExCameraView::OnUpdateSaveImage(CCmdUI* pCmdUI) 
{
	CExCameraDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	pCmdUI->Enable(pDoc->Image().IsValid());
}
