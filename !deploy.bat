@echo off

echo.
echo Deploying library to following locations:
echo.
echo     USER_INCLUDE_PATH = %USER_INCLUDE_PATH%
echo     USER_LIBRARY_PATH = %USER_LIBRARY_PATH%
echo.
echo.


if "%USER_INCLUDE_PATH%" == "" goto ERROR
if "%USER_LIBRARY_PATH%" == "" goto ERROR



copy src\dxfw %USER_INCLUDE_PATH%\dxfw
copy src\dxfw.h %USER_INCLUDE_PATH%
copy lib %USER_LIBRARY_PATH%\



if "%1"=="np" goto END
echo.
echo.
pause


goto END




:ERROR

echo ERROR! - Please set correctly the above enviroment variables!
echo.
echo.

pause


:END
