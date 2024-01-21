@echo off
rem VisXDSReg.bat
regedit /s VisXDS.reg
regsvr32 /s ..\bin\visxrenderfil.ax
regsvr32 /s ..\bin\visxrenderfildb.ax
