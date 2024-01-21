// VisDiagnosticsManager.inl
//
// Copyright © 2000 Microsoft Corporation, All Rights Reserved
//
///////////////////////////////////////////////////////////////////////////


inline void CVisDiagnosticsManager::SetWarningDebugStringLevel(DWORD dwLevel)
{
	VisPGlobals()->SetWarningDebugStringLevel(dwLevel);
}

inline DWORD  CVisDiagnosticsManager::WarningDebugStringLevel(void)
{
	return VisPGlobals()->WarningDebugStringLevel();
}

inline void CVisDiagnosticsManager::SetWarningConsoleLevel(DWORD dwLevel)
{
	VisPGlobals()->SetWarningConsoleLevel(dwLevel);
}
		
inline DWORD  CVisDiagnosticsManager::WarningConsoleLevel(void)
{
	return VisPGlobals()->WarningConsoleLevel();
}

inline void CVisDiagnosticsManager::SetWarningLogLevel(DWORD dwLevel)
{
	VisPGlobals()->SetWarningLogLevel(dwLevel);
}

inline DWORD  CVisDiagnosticsManager::WarningLogLevel(void)
{
	return VisPGlobals()->WarningLogLevel();
}

inline void CVisDiagnosticsManager::SetErrorDebugStringLevel(DWORD dwLevel)
{
	VisPGlobals()->SetErrorDebugStringLevel(dwLevel);
}

inline DWORD  CVisDiagnosticsManager::ErrorDebugStringLevel(void)
{
	return VisPGlobals()->ErrorDebugStringLevel();
}

inline void CVisDiagnosticsManager::SetErrorConsoleLevel(DWORD dwLevel)
{
	VisPGlobals()->SetErrorConsoleLevel(dwLevel);
}

inline DWORD  CVisDiagnosticsManager::ErrorConsoleLevel(void)
{
	return VisPGlobals()->ErrorConsoleLevel();
}

inline void CVisDiagnosticsManager::SetErrorLogLevel(DWORD dwLevel)
{
	VisPGlobals()->SetErrorLogLevel(dwLevel);
}

inline DWORD  CVisDiagnosticsManager::ErrorLogLevel(void)
{
	return VisPGlobals()->ErrorLogLevel();
}

inline void CVisDiagnosticsManager::SetTraceDebugStringLevel(DWORD dwLevel)
{
	VisPGlobals()->SetTraceDebugStringLevel(dwLevel);
}

inline DWORD  CVisDiagnosticsManager::TraceDebugStringLevel(void)
{
	return VisPGlobals()->TraceDebugStringLevel();
}

inline void CVisDiagnosticsManager::SetTraceConsoleLevel(DWORD dwLevel)
{
	VisPGlobals()->SetTraceConsoleLevel(dwLevel);
}

inline DWORD  CVisDiagnosticsManager::TraceConsoleLevel(void)
{
	return VisPGlobals()->TraceConsoleLevel();
}

inline void CVisDiagnosticsManager::SetTraceLogLevel(DWORD dwLevel)
{
	VisPGlobals()->SetTraceLogLevel(dwLevel);
}

inline DWORD  CVisDiagnosticsManager::TraceLogLevel(void)
{
	return VisPGlobals()->TraceLogLevel();
}

inline void CVisDiagnosticsManager::SetProgressDebugStringLevel(DWORD dwLevel)
{
	VisPGlobals()->SetProgressDebugStringLevel(dwLevel);
}

inline DWORD  CVisDiagnosticsManager::ProgressDebugStringLevel(void)
{
	return VisPGlobals()->ProgressDebugStringLevel();
}

inline void CVisDiagnosticsManager::SetProgressConsoleLevel(DWORD dwLevel)
{
	VisPGlobals()->SetProgressConsoleLevel(dwLevel);
}

inline DWORD  CVisDiagnosticsManager::ProgressConsoleLevel(void)
{
	return VisPGlobals()->ProgressConsoleLevel();
}

inline void CVisDiagnosticsManager::SetProgressLogLevel(DWORD dwLevel)
{
	VisPGlobals()->SetProgressLogLevel(dwLevel);
}

inline DWORD  CVisDiagnosticsManager::ProgressLogLevel(void)
{
	return VisPGlobals()->ProgressLogLevel();
}

inline void CVisDiagnosticsManager::EnableDiagnostics(void)
{
	VisPGlobals()->EnableDiagnostics();
}

inline void CVisDiagnosticsManager::DisableDiagnostics(void)
{
	VisPGlobals()->DisableDiagnostics();
}

inline bool CVisDiagnosticsManager::DiagnosticsEnabled(void)
{
	return VisPGlobals()->DiagnosticsEnabled();
}

inline void CVisDiagnosticsManager::EnableConsoleWnd(void)
{
	VisPGlobals()->EnableConsoleWnd();
}

inline void CVisDiagnosticsManager::DisableConsoleWnd(void)
{
	VisPGlobals()->DisableConsoleWnd();
}

inline bool CVisDiagnosticsManager::ShowConsoleWnd(void)
{
	return VisPGlobals()->ShowConsoleWnd();
}


inline void CVisDiagnosticsManager::SetLevelsModified(bool fLevelsModified)
{
	VisPGlobals()->SetDiagLevelsModified(fLevelsModified);
}

inline bool CVisDiagnosticsManager::LevelsModified(void)
{
	return VisPGlobals()->DiagLevelsModified();
}
