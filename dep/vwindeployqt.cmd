:: ------------------------------------------------------------------------------
::    vwindeployqt   ::  windeployqt wrapper for visual studio
::.     
:: ------------------------------------------------------------------------------
:: :: Usage :: vwindeployqt path_to_qt path_to_binary path_to_libdir
:: ::       :: path_to_qt     - path to qt bin
:: ::       :: path_to_binary - path to binary for deployment
:: ::       :: path_to_libdir - path to place where put dll files
::

@echo off
setlocal
set RESULT=0

:checkargs
if "%1" == "/?" goto usage
if "%2" == "" goto usage
if "%3" == "" goto usage

:run
set QT_BIN_DIR=%1
set BINARY=%2
set LIBDIR=%3

set PATH=%QT_BIN_DIR%;%PATH%
windeployqt.exe "%BINARY%" --libdir "%LIBDIR%" -no-translations --no-system-d3d-compiler --no-compiler-runtime
set RESULT=%ERRORLEVEL%

goto end

:usage
echo ------------------------------------------------------------------------------
echo    vwindeployqt   ::  windeployqt wrapper for visual studio
echo.     
echo ------------------------------------------------------------------------------
echo :: Usage :: vwindeployqt path_to_qt path_to_binary path_to_libdir
echo ::       :: path_to_qt     - path to qt bin
echo ::       :: path_to_binary - path to binary for deployment
echo ::       :: path_to_libdir - path to place where put dll files
echo                                        (see http://ss64.com/nt/robocopy.html)
goto end

:end
endlocal
exit /b %RESULT%