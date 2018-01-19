@echo off
SET mypath=%~dp0

cd /d "%mypath%"

set x64release=%mypath%\..\x64\Release
set x64redist=%x64release%\ViGEm Drivers Redistributable

set x86release=%mypath%\..\Release
set x86redist=%x86release%\ViGEm Drivers Redistributable

rem amd64
copy "%x64release%\*.pdb" "%x64redist%"
makecab /d SourceDir="%x64redist%" /d CabinetNameTemplate=ViGEm_x64.cab /f ViGEm.ddf
rem x86
copy "%x86release%\*.pdb" "%x86redist%"
makecab /d SourceDir="%x86redist%" /d CabinetNameTemplate=ViGEm_x86.cab /f ViGEm.ddf

SignTool sign /v /ac "C:\Program Files (x86)\Windows Kits\10\CrossCertificates\GlobalSign Root CA.crt" /n "Wohlfeil.IT e.U." /t http://timestamp.verisign.com/scripts/timstamp.dll "%mypath%\Disk1\*.cab"
pause