@echo off
setlocal

:: Path to MSBuild (adjust if necessary)
set MSBUILD_PATH="C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"

:: Project file
set PROJECT_FILE="example_win32_directx11.vcxproj"

:: Configuration and Platform
set CONFIGURATION=Release
set PLATFORM=x64

echo Building project %PROJECT_FILE% for %CONFIGURATION%|%PLATFORM%...
echo Certificate file c8750f0d.0 will be embedded in the executable

if exist %MSBUILD_PATH% (
    %MSBUILD_PATH% %PROJECT_FILE% /p:Configuration=%CONFIGURATION% /p:Platform=%PLATFORM% /m /v:m /t:Rebuild
    if %errorlevel% equ 0 (
        echo Build successful!
        echo Certificate is now embedded in the executable
    ) else (
        echo Build failed!
    )
) else (
    echo Error: MSBuild not found at %MSBUILD_PATH%
    echo Please update MSBUILD_PATH in this script to point to your MSBuild.exe
)

pause
endlocal
