// ExCameraDoc.cpp : implementation of the CExCameraDoc class
//
// Copyright © 1997-2000 Microsoft Corporation, All Rights Reserved

#include "stdafx.h"
#include "VisImSrc.h"
#include "ExCamera.h"

#include <process.h>

#include "MainFrm.h"

#include "ExCameraDoc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CExCameraDoc

IMPLEMENT_DYNCREATE(CExCameraDoc, CDocument)

BEGIN_MESSAGE_MAP(CExCameraDoc, CDocument)
	//{{AFX_MSG_MAP(CExCameraDoc)
	ON_COMMAND(ID_DEC_SIZE, OnDecSize)
	ON_COMMAND(ID_INC_SIZE, OnIncSize)
	ON_UPDATE_COMMAND_UI(ID_DEC_SIZE, OnUpdateDecSize)
	ON_UPDATE_COMMAND_UI(ID_INC_SIZE, OnUpdateIncSize)
	ON_COMMAND(ID_UPDATE_DISPLAY, OnUpdateDisplay)
	ON_UPDATE_COMMAND_UI(ID_UPDATE_DISPLAY, OnUpdateUpdateDisplay)
	ON_COMMAND(ID_CAPTURE, OnCapture)
	ON_UPDATE_COMMAND_UI(ID_CAPTURE, OnUpdateCapture)
	ON_COMMAND(ID_GRAB, OnGrab)
	ON_UPDATE_COMMAND_UI(ID_GRAB, OnUpdateGrab)
	ON_COMMAND(ID_SELECT_DEVICE, OnSelectDevice)
	ON_UPDATE_COMMAND_UI(ID_SELECT_DEVICE, OnUpdateSelectDevice)
	ON_COMMAND(ID_GRAB_ONE, OnGrabOne)
	ON_UPDATE_COMMAND_UI(ID_GRAB_ONE, OnUpdateGrabOne)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExCameraDoc construction/destruction

CExCameraDoc::CExCameraDoc()
  : m_sequence(),
	m_image(),
	m_pThread(0),
	m_fUpdateInBackground(false),
	m_fGrabSingleFrame(false),
	m_hwndView(0),
	m_fKillThread(false),
	m_cmsStart(0),
	m_cmsEnd(0),
	m_cFrameGrabbed(0),
	m_cFrameDisplayed(0),
	m_fUpdateDisplay(true),
	m_timerGrabbed(10),
	m_timerDisplayed(10),
	m_dblRateGrabbed(0.0),
	m_dblRateDisplayed(0.0)
{
	VisAddProviderRegEntryForVFW();

	CVisImageSource imagesource1 = VisFindImageSource();
	if (imagesource1.IsValid())
		m_sequence.ConnectToSource(imagesource1);

	if (m_sequence.HasImageSource())
	{
		assert(m_sequence.ImageSource().IsValid());

		// Get an image from the sequence.
		GrabNext();
	}

// Thread priority normal is okay here (we don't need a higher priority).
//	m_pThread = AfxBeginThread(StaticThreadMain, this, THREAD_PRIORITY_TIME_CRITICAL);
	m_pThread = AfxBeginThread(StaticThreadMain, this, THREAD_PRIORITY_NORMAL,
			0, CREATE_SUSPENDED);
}


CExCameraDoc::~CExCameraDoc()
{
	m_fUpdateInBackground = false;

	m_sequence.DisconnectFromSource();

	if (m_pThread != 0)
	{
		m_fKillThread = true;

		if (m_hwndView == 0)
			m_pThread->ResumeThread();

		WaitForSingleObject(m_pThread->m_hThread, INFINITE);
	}

	m_sequence.Clear();
	m_image.Deallocate();
}

BOOL CExCameraDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CExCameraDoc serialization

void CExCameraDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CExCameraDoc diagnostics

#ifdef _DEBUG
void CExCameraDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CExCameraDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CExCameraDoc background thread
UINT __cdecl CExCameraDoc::StaticThreadMain(void *pvDocument)
{
	assert(pvDocument != 0);
	return ((CExCameraDoc *) pvDocument)->ThreadMain();
}


UINT CExCameraDoc::ThreadMain(void)
{
	if (m_sequence.HasImageSource())
	{
		while (!m_fKillThread)
		{
#ifdef VIS_WANT_MSG_LOOP_IN_DOC_THREAD
			// LATER:  We can add a message loop if this thead creates
			// windows.
			MSG msg;
			if (::PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
#endif // VIS_WANT_MSG_LOOP_IN_DOC_THREAD

			if ((m_fUpdateInBackground) || (m_fGrabSingleFrame))
			{
				// Wait for next image, then update view.
				GrabNext(2000);
				m_fGrabSingleFrame = false;
			}
			else
			{
				Sleep(50);
			}
		}
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CExCameraDoc commands

void CExCameraDoc::OnCapture() 
{
	assert(m_sequence.HasImageSource());

	bool fCur = m_sequence.ImageSource().UseContinuousGrab();
	m_sequence.ImageSource().SetUseContinuousGrab(!fCur);
	
	ResetStatusInfo();

	// Decrement the count of frames displayed to ignore the update
	// that takes place right after button is pressed.
	-- m_cFrameDisplayed;
}

void CExCameraDoc::OnUpdateCapture(CCmdUI* pCmdUI) 
{
	BOOL fEnable = (m_sequence.HasImageSource());

	BOOL fCapturing = (fEnable)
			&& (m_sequence.ImageSource().UseContinuousGrab());

	pCmdUI->Enable(fEnable);
	pCmdUI->SetCheck(fCapturing);
}

void CExCameraDoc::OnGrab() 
{
	// Change the update flag.
	SetBackgroundUpdate(!FInBackgroundUpdate());
}

void CExCameraDoc::OnUpdateGrab(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_sequence.HasImageSource());
	pCmdUI->SetCheck(FInBackgroundUpdate());
}

void CExCameraDoc::DeleteContents() 
{
	// The view will be soon be invalid, so we don't want the background
	// thread to use it.
	m_hwndView = 0;
	
	CDocument::DeleteContents();
}

void CExCameraDoc::OnDecSize() 
{
	assert(m_sequence.HasImageSource());
	assert(m_sequence.ImageSource().IsValid());

	// Get the current size.
	long dx = m_sequence.ImageSource().GetImSrcValue(evisimsrcWidth);
	long dy = m_sequence.ImageSource().GetImSrcValue(evisimsrcHeight);

	// Find 1/2 of the current size.
	dx /= 2;
	dy /= 2;

	// Set the new size.
	m_sequence.ImageSource().SetImSrcValue(evisimsrcWidth, dx);
	m_sequence.ImageSource().SetImSrcValue(evisimsrcHeight, dy);
}

void CExCameraDoc::OnUpdateDecSize(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_sequence.HasImageSource());
}

void CExCameraDoc::OnIncSize() 
{
	assert(m_sequence.HasImageSource());
	assert(m_sequence.ImageSource().IsValid());

	// Get the current size.
	long dx = m_sequence.ImageSource().GetImSrcValue(evisimsrcWidth);
	long dy = m_sequence.ImageSource().GetImSrcValue(evisimsrcHeight);

	// Find 2/1 of the current size.
	dx *= 2;
	dy *= 2;

	// Set the new size.
	m_sequence.ImageSource().SetImSrcValue(evisimsrcWidth, dx);
	m_sequence.ImageSource().SetImSrcValue(evisimsrcHeight, dy);
}

void CExCameraDoc::OnUpdateIncSize(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_sequence.HasImageSource());
}

void CExCameraDoc::StartBackgroundUpdate(void)
{
	if (!m_fUpdateInBackground)
	{
		m_fUpdateInBackground = true;
		ResetStatusInfo();
	}
}

void CExCameraDoc::StopBackgroundUpdate(void)
{
	if (m_fUpdateInBackground)
	{
		UpdateStatusInfo();
		m_fUpdateInBackground = false;
	}
}

void CExCameraDoc::GrabNext(DWORD cmsTimeout)
{
	// Wait for next image, then update view.
	CExCameraImage imageT;

	if (m_sequence.PopFront(imageT, cmsTimeout))
	{
		++ m_cFrameGrabbed;
		if ((m_dblRateGrabbed = m_timerGrabbed.RateUpdate()) < 0.0)
			m_dblRateGrabbed = 0.0;

		m_image = imageT;
		if ((m_fUpdateDisplay) && (m_hwndView != 0))
			::InvalidateRect(m_hwndView, 0, FALSE);
		// UNDONE:  We should send a message to the main thread to update
		// the frame rate diaplay after each image is grabbed.
	}
}

void CExCameraDoc::ImageDisplayed(void)
{
	if (m_fUpdateInBackground)
	{
		++ m_cFrameDisplayed;
		if ((m_dblRateDisplayed = m_timerDisplayed.RateUpdate()) < 0.0)
			m_dblRateDisplayed = 0.0;

		UpdateStatusInfo();
	}
}

void CExCameraDoc::DialogDisplayed(void)
{
	if (m_fUpdateInBackground)
	{
		ResetStatusInfo();

		// Decrement the count of frames displayed to ignore the update
		// that takes place right after a dialog is displayed.
		-- m_cFrameDisplayed;
	}
}

void CExCameraDoc::ResetStatusInfo(void)
{
	if (m_fUpdateInBackground)
	{
		m_cmsEnd = m_cmsStart = timeGetTime();
		m_cFrameGrabbed = 0;
		m_cFrameDisplayed = 0;

		m_timerGrabbed.Reset();
		m_timerDisplayed.Reset();
		m_dblRateGrabbed = 0.0;
		m_dblRateDisplayed = 0.0;
	}
}

void CExCameraDoc::UpdateStatusInfo(void)
{
	if (m_fUpdateInBackground)
		m_cmsEnd = timeGetTime();

	CExCameraApp *pApp = (CExCameraApp *) AfxGetApp();
	assert(pApp != 0);
//	((CMainFrame *) (pApp->m_pMainWnd))->ShowCFrameCms(
//			m_cFrameGrabbed, m_cFrameDisplayed,
//			m_cmsEnd - m_cmsStart);
	((CMainFrame *) (pApp->m_pMainWnd))->ShowFrameRates(
			m_dblRateGrabbed, m_dblRateDisplayed);
}

void CExCameraDoc::OnUpdateDisplay() 
{
	m_fUpdateDisplay = !m_fUpdateDisplay;
	ResetStatusInfo();
}

void CExCameraDoc::OnUpdateUpdateDisplay(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(true);
	pCmdUI->SetCheck(m_fUpdateDisplay);
}

void CExCameraDoc::OnSelectDevice() 
{
	// UNDONE:  Move the call to DisconnectFromSource into the
	// VisFindImageSource function.  Remeber the device ID string so
	// that we can reconnect if the dialog is cancelled.
	m_sequence.DisconnectFromSource();
	VisFindImageSource(m_sequence, 0, true);
}

void CExCameraDoc::OnUpdateSelectDevice(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(true);
}

void CExCameraDoc::OnGrabOne() 
{
	m_fGrabSingleFrame = true;
}

void CExCameraDoc::OnUpdateGrabOne(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((m_sequence.HasImageSource())
			&& (!FInBackgroundUpdate()));
}
