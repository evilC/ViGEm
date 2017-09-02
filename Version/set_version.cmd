@echo off
SET mypath=%~dp0
SET solution=%mypath:~0,-1%\..

rem HidCerberus Library
honestus --version-from-file version.txt --target-file "%solution%\Src\HidCerberus.Lib\HidCerberus.Lib.rc" --resource.file-version --resource.product-version

rem HidCerberus Server
honestus --version-from-file version.txt --target-file "%solution%\NET\HidCerberus.Srv\Properties\AssemblyInfo.cs" --assembly.version --assembly.file-version

rem HidGuardian Driver
honestus --version-from-file version.txt --target-file "%solution%\Sys\HidGuardian\HidGuardian.rc" --resource.file-version --resource.product-version
honestus --version-from-file version.txt --target-file "%solution%\Sys\HidGuardian\HidGuardian.vcxproj" --vcxproj.inf-time-stamp

rem ViGEm Bus Driver
honestus --version-from-file version.txt --target-file "%solution%\Sys\ViGEmBus\ViGEmBus.rc" --resource.file-version --resource.product-version
honestus --version-from-file version.txt --target-file "%solution%\Sys\ViGEmBus\ViGEmBus.vcxproj" --vcxproj.inf-time-stamp

rem VDX Application
honestus --version-from-file version.txt --target-file "%solution%\Src\Samples\VDX\VDX.rc" --resource.file-version --resource.product-version

rem XnaGuardian Driver
honestus --version-from-file version.txt --target-file "%solution%\Sys\XnaGuardian\XnaGuardian.rc" --resource.file-version --resource.product-version
honestus --version-from-file version.txt --target-file "%solution%\Sys\XnaGuardian\XnaGuardian.vcxproj" --vcxproj.inf-time-stamp

rem XInputExtensions Library
honestus --version-from-file version.txt --target-file "%solution%\Src\XInputExtensions\XInputExtensions.rc" --resource.file-version --resource.product-version

rem ViGEmUM.NET Library
honestus --version-from-file version.txt --target-file "%solution%\ViGEmUM.NET\Properties\AssemblyInfo.cs" --assembly.version --assembly.file-version

rem ViGEmClient Library
honestus --version-from-file version.txt --target-file "%solution%\Src\ViGEmClient\ViGEmClient.rc" --resource.file-version --resource.product-version
