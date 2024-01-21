// ExMFCOpenSave.cpp : Defines the class behaviors for the application.
//
// Copyright © 1997-2000 Microsoft Corporation, All Rights Reserved

#include "stdafx.h"
#include "VisCore.h"
#include "VisMemoryChecks.h"

#include "ExMFCOpenSave.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "ExMFCOpenSaveDoc.h"
#include "ExMFCOpenSaveView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExMFCOpenSaveApp

BEGIN_MESSAGE_MAP(CExMFCOpenSaveApp, CWinApp)
	//{{AFX_MSG_MAP(CExMFCOpenSaveApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FULL_SCREEN_PRINT, OnFullScreenPrint)
	ON_UPDATE_COMMAND_UI(ID_FULL_SCREEN_PRINT, OnUpdateFullScreenPrint)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExMFCOpenSaveApp construction

CExMFCOpenSaveApp::CExMFCOpenSaveApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	_onexit(CVisGlobals::OnExit);
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CExMFCOpenSaveApp object

CExMFCOpenSaveApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CExMFCOpenSaveApp initialization

BOOL CExMFCOpenSaveApp::InitInstance()
{
	m_wPercentPrintPage = 0;
	
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings(10);  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_EXMFCOTYPE,
		RUNTIME_CLASS(CExMFCOpenSaveDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CExMFCOpenSaveView));
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CExMFCOpenSaveApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CExMFCOpenSaveApp commands

void CExMFCOpenSaveApp::OnFullScreenPrint() 
{
	m_wPercentPrintPage = ((FScaleWhenPrinting()) ? 0 : 100);
}

void CExMFCOpenSaveApp::OnUpdateFullScreenPrint(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(true);
	pCmdUI->SetCheck(FScaleWhenPrinting());
}
