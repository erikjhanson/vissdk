# Microsoft Developer Studio Project File - Name="Examples" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=Examples - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Examples.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Examples.mak" CFG="Examples - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Examples - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "Examples - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0

!IF  "$(CFG)" == "Examples - Win32 Release"

# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f Examples.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "Examples.exe"
# PROP BASE Bsc_Name "Examples.bsc"
# PROP BASE Target_Dir ""
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Temp"
# PROP Intermediate_Dir "..\Temp"
# PROP Cmd_Line "NMAKE /f ..\VisProj.mak VISBUILD=exrel OUTDIR=..\Temp"
# PROP Rebuild_Opt "/a"
# PROP Target_File "..\Temp\buildexrel.snt"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "Examples - Win32 Debug"

# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f Examples.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "Examples.exe"
# PROP BASE Bsc_Name "Examples.bsc"
# PROP BASE Target_Dir ""
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Temp"
# PROP Intermediate_Dir "..\Temp"
# PROP Cmd_Line "NMAKE /f ..\VisProj.mak VISBUILD=exdbg OUTDIR=..\Temp"
# PROP Rebuild_Opt "/a"
# PROP Target_File "..\Temp\buildexdbg.snt"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "Examples - Win32 Release"
# Name "Examples - Win32 Debug"

!IF  "$(CFG)" == "Examples - Win32 Release"

!ELSEIF  "$(CFG)" == "Examples - Win32 Debug"

!ENDIF 

# End Target
# End Project
