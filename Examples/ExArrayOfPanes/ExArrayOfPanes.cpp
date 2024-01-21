///////////////////////////////////////////////////////////////////////////
//
// NAME
//  ExArrayOfPanes.cpp
//
// DESCRIPTION
//  Basic MFC Application example using the image and display classes.  It
//  creates and displays a couple images.
//
// SEE ALSO
//
// BUGS
//
// DESIGN
//  This program continues to run and handle events after it leaves InitInstance(),
//  because (1) InitInstance() returnes TRUE and (2) the member variable m_pMainWnd
//  is set to a valid CWnd-derived window object.
//
// HISTORY
//  19-Jul-96 Vision Technology Group, Microsoft Research, Microsoft Corp.
//
// Copyright © 1996-2000 Microsoft Corporation, All Rights Reserved
//
///////////////////////////////////////////////////////////////////////////


// MSVisSDK include files
// <afxwin.h> and "VisCore.h" are included in "VisDisplay.h".
// UNDONE:  Add inc directory to the path.
#include "VisWin.h"
#include "VisDisplay.h"
#include "VisMemoryChecks.h"


#include "VisSampleImages.h"


// LATER:  Define this to test the open image dialog.
//#define TEST_OPEN_DLG 1

#ifdef TEST_OPEN_DLG
#include "..\..\VisDisplay\VisOpenImageDlg.h"
#endif // TEST_OPEN_DLG


#ifdef _DEBUG
#undef new
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _DEBUG



// We'll use this type in an image that can't be displayed.
typedef struct { int m_rgi[5]; double m_dbl; EVisPixFmt m_evispixel; }
		T_CantDisplay;


class CMyWinApp : public CWinApp
{
public:
	enum { enCols = 256 };
	enum { enRows = 256 };

    BOOL InitInstance();
};


void VISAPI EventCallbackHi(UINT flags, CPoint point, CVisPaneWnd *ppanewnd,
		void *pvUser)
{
	ppanewnd->RemoveMouseEventCallback(EventCallbackHi);
    AfxMessageBox("Hi!");
}


void VISAPI EventCallbackDelete(UINT flags, CPoint point,
		CVisPaneWnd *ppanewnd, void *pvUser)
{
    assert(pvUser == (void *) 5);
	
	ppanewnd->Clear();
	ppanewnd->RemoveMouseEventCallback(EventCallbackDelete);
}


void VISAPI EventCallbackSelectToggleAll(UINT flags, CPoint point,
		CVisPaneWnd *ppanewnd, void *pvUser)
{
	assert(ppanewnd->IsPartOfArray());
	assert(ppanewnd->PWndParent() != 0);
	CVisPaneArrayWnd *ppanearraywnd
			= (CVisPaneArrayWnd *) ppanewnd->PWndParent();

	for (int i = 0; i < ppanearraywnd->NPanes(); ++i)
	{
        ppanearraywnd->Pane(i).SetIsSelected(
				!ppanearraywnd->Pane(i).IsSelected());
        ppanearraywnd->Pane(i).Refresh();
    }
}


void VISAPI EventCallbackSelectToggleCur(UINT flags, CPoint point,
		CVisPaneWnd *ppanewnd, void *pvUser)
{
	ppanewnd->SetIsSelected(!ppanewnd->IsSelected());
	ppanewnd->Refresh();
}


void VISAPI EventCallbackXY(UINT flags, CPoint point,
		CVisPaneWnd *ppanewnd, void *pvUser)
{
    CString str;
    str.Format("%d, %d", point.x, point.y);
    AfxMessageBox(str);
}


BOOL CMyWinApp::InitInstance()
{
#ifdef TEST_OPEN_DLG

	char *sFiles[5];

	CVisOpenImageDlg dlg(5,sFiles);
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;

#endif TEST_OPEN_DLG
	
	// Code for testing panes.
	char szFullPath[256];
	VisGetFullPathForSample(szFullPath, "Leaves4.bmp");
    try
    {
		//------------------------------------------------------------------
		// Create the pane array

		// UNDONE:  Change code so that window is created after images have
		// been given to panes.
		CVisPaneArray panearray(4, 3,
					evispaneAutoTitle
					| evispanePaneTitle
					| evispanePaneBorder
					| evispaneArrayTitle
					| evispaneArrayBorder
					| evispaneStatusDefault
					| evispaneCanResize
					| evispaneCanMaximize
					| evispaneVisible
					| evispaneSelectHandler
					| evispaneShowValues);
		panearray.SetTitle("VisSDK Array of Panes example program");
		panearray.SetSizePaneInside(enCols, enRows);


		//------------------------------------------------------------------
		// Simple color image examples

		// Create a color image by specifying the width and height.
		CVisRGBAByteImage imageC0(enCols, enRows * 3 / 4);

		// Display the image in pane 0.  Give an alias to the pane, so that
		// later changes to the image will appear when the pane is refreshed.
        panearray.Pane(0).Display(imageC0, evispanedispAliasOrigNoScrnBuf);

		// Create a color image that is a subimage of our original color image.
		// The rectangle is specified by its left, top, right, and bottom
		// coordinates in the original image.  Note that the subimage will use
		// the same coordinates as the original image
		CVisRGBAByteImage imageC1 = imageC0.SubImage(enCols / 8, enRows / 8,
				enCols * 3 / 8, enRows * 3 / 8);  // l, t, r, b
		
		// Give the subimage a name.  (The original image did not have a name.
		// This will not change the name of the original image.)
		imageC1.SetName("Color Image");

		// Fill the pixels in the original image with a yellow color.
		CVisRGBABytePixel rgbabT(240, 200, 0, 255);
		imageC0.FillPixels(rgbabT);

		// Display a copy of the original image in pane 4.
        panearray.Pane(4).Display(imageC0, evispanedispCopyOrigNoScrnBuf);
		panearray.Pane(3).SetTitle("Copy of original color image");

		// Fill the pixels in the subimage with a red color.
		rgbabT.SetRGB(255, 0, 100);
		imageC1.FillPixels(rgbabT);

		// Display (a copy of) the subimage in pane 1.
        panearray.Pane(1).Display(imageC1);

		// Use the SubImage method to create another alias of a part of the
		// original image.  (This rectangle is larger than the rectangle for
		// the first subimage, but still smaller than the original image
		// rectangle.)
		CVisRGBAByteImage imageC2 = imageC1.SubImage(enCols / 10, enRows / 10,
				enCols / 2, enRows / 2);

		// Display (a copy of) the second subimage in pane 2.
        panearray.Pane(2).Display(imageC2);

		// Display (a copy of) the second subimage in pane 3.  The point
		// specifies the position in the pane when the top-left corner of
		// the subimage will be displayed.
		panearray.Pane(3).Display(imageC2, CPoint(enCols / 10, enRows / 10));
		panearray.Pane(3).SetTitle("Offset color image");


		//------------------------------------------------------------------
		// A simple byte image example

		CVisByteImage imageB1(100,100);
		imageB1.SetName("Boring byte image");
		imageB1.FillPixels(200);
		imageB1.SetName("Byte Image #2");
		panearray.Pane(5).SetIsSelected(true);
		panearray.Pane(5).Display(imageB1);
		imageB1.SetName("Byte Image #2a");


		//------------------------------------------------------------------
		// An image from a file

        CVisRGBAByteImage imageFromFile;
        imageFromFile.FReadFile(szFullPath);
        panearray.Pane(6).Display(imageFromFile, evispanedispNoOrigNoScrnBuf);


		//------------------------------------------------------------------
		// An image that can't be displayed

		CVisImage<T_CantDisplay> imageCantDisp(50, 50);
		imageCantDisp.SetName("Can't display this image.  (Ignore the error.)");
		panearray.Pane(7).Display(imageCantDisp);


		//------------------------------------------------------------------
		// Normalization options used with standard numeric types

		CVisFloatImage imageFloat1(enCols, enRows);
		for (int iRow = 0; iRow < imageFloat1.Height(); iRow++)
			for (int iCol = 0; iCol < imageFloat1.Width(); iCol++)
				imageFloat1.Pixel(iCol, iRow) = (float) (iCol - 40);
		imageFloat1.SetName("Float image, iCol - 40, Relative");
		panearray.Pane(8).Display(evisnormalizeRelative, imageFloat1);
		imageFloat1.SetName("Float image, iCol - 40, Absolute");
		panearray.Pane(9).Display(evisnormalizeAbsolute, imageFloat1);
		imageFloat1.SetName("Float image, iCol - 40, Relative Magnitude");
		panearray.Pane(10).Display(evisnormalizeRelMagnitude, imageFloat1);
		imageFloat1.SetName("Float image, iCol - 40, Absolute Magnitude");
		panearray.Pane(11).Display(evisnormalizeAbsMagnitude, imageFloat1);


		//------------------------------------------------------------------
		// Add some points and lines to some of the images

		panearray.Pane(2).AddPoint(30, 30, PS_SOLID, 5);
		panearray.Pane(2).AddLine(25, 25, 25, 50);
		panearray.Pane(2).AddLine(25, 25, 50, 25);

		panearray.Pane(3).AddPoint(30, 30, PS_SOLID, 5);
		panearray.Pane(3).AddLine(25, 25, 25, 50);
		panearray.Pane(3).AddLine(25, 25, 50, 25);

		panearray.Pane(9).AddPoint(10, 10);
		panearray.Pane(9).AddPoint(20, 20, PS_SOLID, 5);
		panearray.Pane(9).AddLine(20, 10, 10, 20);
		panearray.Pane(9).AddLine(0, 0, 0, 50);
		panearray.Pane(9).AddLine(0, 0, 50, 0);


		//------------------------------------------------------------------
		// Event handlers

		panearray.Pane(0).AddMouseEventCallback(
				evismouseLButtonDown | evismouseControl,
				EventCallbackSelectToggleAll, "EventCallbackSelectToggleAll");
		panearray.Pane(1).AddMouseEventCallback(evismouseLButtonDown,
				CVisPaneWnd::SelectPaneCallback,
				"CVisPaneWnd::SelectPaneCallback");
		panearray.Pane(2).AddMouseEventCallback(evismouseLButtonUp,
				EventCallbackXY);
		panearray.Pane(3).AddMouseEventCallback(evismouseLButtonDown,
				EventCallbackDelete, "Delete", (void *) 5);
		panearray.Pane(4).AddMouseEventCallback(evismouseLDoubleClick,
				EventCallbackHi);


#ifdef VIS_TEST_PADDING
		//------------------------------------------------------------------
		// Padding test

        CVisRGBAByteImage imageT(imageFromFile);
		CRect rectPad(imageT.Left() + 50, imageT.Top() + 50,
				imageT.Right() - 50, imageT.Bottom() - 50);
		if ((rectPad.Width() > 0) && (rectPad.Height() > 0))
		{
			imageT.Pad(evispadZero, rectPad);
			panearray.Pane(0).Display(imageT);
			panearray.Pane(0).AddLine(imageT.Left(), rectPad.top,
					imageT.Right(), rectPad.top);
			panearray.Pane(0).AddLine(imageT.Left(), rectPad.bottom,
					imageT.Right(), rectPad.bottom);
			panearray.Pane(0).AddLine(rectPad.left, imageT.Top(),
					rectPad.left, imageT.Bottom());
			panearray.Pane(0).AddLine(rectPad.right, imageT.Top(),
					rectPad.right, imageT.Bottom());

			imageT.Pad(evispadReplicate, rectPad);
			panearray.Pane(1).Display(imageT);
			panearray.Pane(1).AddLine(imageT.Left(), rectPad.top,
					imageT.Right(), rectPad.top);
			panearray.Pane(1).AddLine(imageT.Left(), rectPad.bottom,
					imageT.Right(), rectPad.bottom);
			panearray.Pane(1).AddLine(rectPad.left, imageT.Top(),
					rectPad.left, imageT.Bottom());
			panearray.Pane(1).AddLine(rectPad.right, imageT.Top(),
					rectPad.right, imageT.Bottom());

			imageT.Pad(evispadReflect, rectPad);
			panearray.Pane(2).Display(imageT);
			panearray.Pane(2).AddLine(imageT.Left(), rectPad.top,
					imageT.Right(), rectPad.top);
			panearray.Pane(2).AddLine(imageT.Left(), rectPad.bottom,
					imageT.Right(), rectPad.bottom);
			panearray.Pane(2).AddLine(rectPad.left, imageT.Top(),
					rectPad.left, imageT.Bottom());
			panearray.Pane(2).AddLine(rectPad.right, imageT.Top(),
					rectPad.right, imageT.Bottom());

			imageT.Pad(evispadCyclic, rectPad);
			panearray.Pane(3).Display(imageT);
			panearray.Pane(3).AddLine(imageT.Left(), rectPad.top,
					imageT.Right(), rectPad.top);
			panearray.Pane(3).AddLine(imageT.Left(), rectPad.bottom,
					imageT.Right(), rectPad.bottom);
			panearray.Pane(3).AddLine(rectPad.left, imageT.Top(),
					rectPad.left, imageT.Bottom());
			panearray.Pane(3).AddLine(rectPad.right, imageT.Top(),
					rectPad.right, imageT.Bottom());
		}
#endif // VIS_TEST_PADDING


		// Set m_pMainWnd so that the event loop is entered after leaving
		// InitInstance().
		m_pMainWnd = panearray.PWnd();
    }
    catch (CVisError& error)
    {
        AfxMessageBox(error.FullMessage());
    }

	return (m_pMainWnd != 0);
}


CMyWinApp vapp;
