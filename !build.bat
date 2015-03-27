@echo off
call vcvars32.bat

del /Q lib\*.* >nul 2>nul

echo.
echo.
echo ---------------- %cd% - Debug ----------------
echo.
echo.
msbuild /clp:NoSummary;ErrorsOnly /p:"VCBuildAdditionalOptions= /useenv" /p:Configuration=Debug /p:Platform=Win32
if %errorlevel%==0 goto OK1
set FAILED=1
:OK1

echo.
echo.
echo ---------------- %cd% - Release ----------------
echo.
echo.
msbuild /clp:NoSummary;ErrorsOnly /p:"VCBuildAdditionalOptions= /useenv" /p:Configuration=Release /p:Platform=Win32
if %errorlevel%==0 goto OK1
set FAILED=1
:OK1

if "%FAILED%"=="" goto OK
echo.
echo.
echo FAILED: Project %cd% build
echo.
echo.
pause
goto END

:OK
echo.
echo.
echo SUCCESS: Project %cd% build
echo.
echo.
call !deploy.bat np
call !deploy1.bat np

:END
