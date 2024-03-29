# Microsoft Developer Studio Project File - Name="VisMatrixLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=VisMatrixLib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "VisMatrixLib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "VisMatrixLib.mak" CFG="VisMatrixLib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "VisMatrixLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "VisMatrixLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "VisMatrixLib - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Lib"
# PROP Intermediate_Dir "RelLib"
# PROP Target_Dir ""
F90=fl32.exe
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "VIS_SDK_LIBS" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "VIS_MATRIX_BUILD" /D "VIS_INCLUDE_SDSTREAM" /YX"VisMatrixPch.h" /Fd"..\Lib/VisMatrixLib" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "VisMatrixLib - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Lib"
# PROP Intermediate_Dir "DbgLib"
# PROP Target_Dir ""
F90=fl32.exe
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /D "_DEBUG" /D "VIS_SDK_LIBS" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "VIS_MATRIX_BUILD" /D "VIS_INCLUDE_SDSTREAM" /YX"VisMatrixPch.h" /Fd"..\Lib/VisMatrixLibDB" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Lib\VisMatrixLibDB.lib"

!ENDIF 

# Begin Target

# Name "VisMatrixLib - Win32 Release"
# Name "VisMatrixLib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\VisCovariance.cpp
# End Source File
# Begin Source File

SOURCE=.\VisDMatrix.cpp
# End Source File
# Begin Source File

SOURCE=.\VisTransform4x4.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\VisCovariance.h
# End Source File
# Begin Source File

SOURCE=.\VisDMatrix.h
# End Source File
# Begin Source File

SOURCE=.\VisDMatrix.inl
# End Source File
# Begin Source File

SOURCE=.\VisLuciformWrappers.h
# End Source File
# Begin Source File

SOURCE=.\VisLuciformWrappers.inl
# End Source File
# Begin Source File

SOURCE=.\VisMatrixProj.h
# End Source File
# Begin Source File

SOURCE=.\VisTransform4x4.h
# End Source File
# Begin Source File

SOURCE=.\VisTransform4x4.inl
# End Source File
# End Group
# End Target
# End Project
