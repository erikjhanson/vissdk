// ExMFCOpenSaveView.cpp : implementation of the CExMFCOpenSaveView class
//
// Copyright © 1997-2000 Microsoft Corporation, All Rights Reserved


// LATER:  Zooming, initial image POINT offsets (when StartPoint() != (0, 0)),
// and titles when printing.


#include "stdafx.h"
#include "VisCore.h"
#include "ExMFCOpenSave.h"

#include "ExMFCOpenSaveDoc.h"
#include "ExMFCOpenSaveView.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExMFCOpenSaveView

IMPLEMENT_DYNCREATE(CExMFCOpenSaveView, CScrollView)

BEGIN_MESSAGE_MAP(CExMFCOpenSaveView, CScrollView)
	//{{AFX_MSG_MAP(CExMFCOpenSaveView)
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExMFCOpenSaveView construction/destruction

CExMFCOpenSaveView::CExMFCOpenSaveView()
{
}

CExMFCOpenSaveView::~CExMFCOpenSaveView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CExMFCOpenSaveView drawing

void CExMFCOpenSaveView::OnDraw(CDC* pDC)
{
	CExMFCOpenSaveDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CVisImageBase& refimage = pDoc->Image();
	
	assert(refimage.IsValid());

	refimage.DisplayInHdc(*pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CExMFCOpenSaveView printing

BOOL CExMFCOpenSaveView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CExMFCOpenSaveView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CExMFCOpenSaveView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CExMFCOpenSaveView diagnostics

#ifdef _DEBUG
void CExMFCOpenSaveView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CExMFCOpenSaveView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CExMFCOpenSaveDoc* CExMFCOpenSaveView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CExMFCOpenSaveDoc)));
	return (CExMFCOpenSaveDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CExMFCOpenSaveView message handlers

void CExMFCOpenSaveView::OnInitialUpdate() 
{
	// Need to set the document size.
	// MFC Example says that MM_LOENGLISH is better than MM_TEXT for
	// printing.
	// Note:  We could also add page and line sizes to SetScrollSize.
	CExMFCOpenSaveDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CVisImageBase& refimage = pDoc->Image();
	
	if (refimage.IsValid())
	{
		CSize size(refimage.Size());
		SetScrollSizes(MM_TEXT, size);
		ResizeParentToFit(false);

		// May need to reduce the view's frame window size to make
		// it fit in its parent's client area.
		CFrameWnd* pFrame = GetParentFrame();
		if (pFrame != NULL)
		{
			CRect rectSized;
			pFrame->GetWindowRect(&rectSized);

			CWnd *pParent = pFrame->GetParent();
			if (pParent != 0)
			{
				// May need to adjust rect so that it fits in parent.
				bool fResized = false;

				pParent->ScreenToClient(rectSized);

				CRect rectParent;
				pParent->GetClientRect(&rectParent);

				assert(rectParent.left <= rectSized.left);
				assert(rectParent.top <= rectSized.top);

				if (rectParent.right < rectSized.right)
				{
					rectSized.right = rectParent.right;
					rectSized.bottom += GetSystemMetrics(SM_CYVSCROLL) + 1;
					fResized = true;
				}

				if (rectParent.bottom < rectSized.bottom)
				{
					rectSized.bottom = rectParent.bottom;
					rectSized.right += GetSystemMetrics(SM_CXHSCROLL) + 1;
					if (rectParent.right < rectSized.right)
						rectSized.right = rectParent.right;
					fResized = true;
				}

				if (fResized)
					pFrame->MoveWindow(&rectSized);
			}
		}
	}

	CScrollView::OnInitialUpdate();
}

BOOL CExMFCOpenSaveView::OnEraseBkgnd(CDC* pDC) 
{
    // This probably isn't needed if the windows are sized correctly,
	// but it might improve performance a little.
	CBrush br( GetSysColor( COLOR_WINDOW ) ); 
    FillOutsideRect( pDC, &br );
	
	return CScrollView::OnEraseBkgnd(pDC);
}

void CExMFCOpenSaveView::OnMouseMove(UINT nFlags, CPoint point) 
{
	CRect rect;
	GetClientRect(&rect);
	if (rect.PtInRect(point))
	{
		CExMFCOpenSaveDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);

		CVisImageBase& refimage = pDoc->Image();

		CPoint pointImage = point + GetDeviceScrollPosition();

		if (refimage.Rect().PtInRect(pointImage))
		{
			CMainFrame *pMainFrame = ((CMainFrame *) AfxGetApp()->m_pMainWnd);

			char szValue[256];
			refimage.GetPixelValueString(pointImage, szValue, sizeof(szValue));
	
			CString text;
			text.Format("(%3d, %3d)  -> [%s]",
					pointImage.x, pointImage.y, szValue);

			pMainFrame->SetMessageText(text);
		}
	}
	
	CScrollView::OnMouseMove(nFlags, point);
}

void CExMFCOpenSaveView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	CExMFCOpenSaveApp *pApp = (CExMFCOpenSaveApp *) AfxGetApp();
	ASSERT_VALID(pApp);
	CExMFCOpenSaveDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CVisImageBase& refimage = pDoc->Image();
	
	if (refimage.IsValid())
	{
		assert(pDC != 0);
		assert(pInfo != 0);

		int w = GetDeviceCaps(*pDC, RASTERCAPS);
		if (((RC_STRETCHDIB & w) != 0)
				&& (pApp->FScaleWhenPrinting()))
		{
			// Scale the image when printing.
			CSize sizeImage(refimage.Size());
			CSize sizePage(pInfo->m_rectDraw.Size());
			CRect rectDest(pInfo->m_rectDraw);

			if (sizeImage.cy * sizePage.cx < sizeImage.cx * sizePage.cy)
			{
				// X-dimensions determine scale
				rectDest.bottom = rectDest.top
						+ sizeImage.cy * sizePage.cx / sizeImage.cx;
			}
			else
			{
				// Y-dimensions determine scale
				rectDest.right = rectDest.left
						+ sizeImage.cx * sizePage.cy / sizeImage.cy;
			}

			rectDest.right = rectDest.left + rectDest.Width()
					* pApp->PercentPrintPage() / 100;
			rectDest.bottom = rectDest.top + rectDest.Height()
					* pApp->PercentPrintPage() / 100;
			
			refimage.DisplayInHdc(*pDC, refimage.Rect(), rectDest);
		}
		else
		{
			// Don't scale when printing.
			refimage.DisplayInHdc(*pDC);
		}
	}
}
