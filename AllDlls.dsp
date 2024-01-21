# Microsoft Developer Studio Project File - Name="AllDlls" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=AllDlls - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AllDlls.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AllDlls.mak" CFG="AllDlls - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AllDlls - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "AllDlls - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0

!IF  "$(CFG)" == "AllDlls - Win32 Release"

# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f AllDlls.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "AllDlls.exe"
# PROP BASE Bsc_Name "AllDlls.bsc"
# PROP BASE Target_Dir ""
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "NMAKE /f VisProj.mak VISBUILD=alldllsrel"
# PROP Rebuild_Opt "/a"
# PROP Target_File "Temp\buildalldllsrel.snt"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "AllDlls - Win32 Debug"

# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f AllDlls.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "AllDlls.exe"
# PROP BASE Bsc_Name "AllDlls.bsc"
# PROP BASE Target_Dir ""
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "NMAKE /f VisProj.mak VISBUILD=alldllsdbg"
# PROP Rebuild_Opt "/a"
# PROP Target_File "Temp\buildalldllsdbg.snt"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "AllDlls - Win32 Release"
# Name "AllDlls - Win32 Debug"

!IF  "$(CFG)" == "AllDlls - Win32 Release"

!ELSEIF  "$(CFG)" == "AllDlls - Win32 Debug"

!ENDIF 

# End Target
# End Project
