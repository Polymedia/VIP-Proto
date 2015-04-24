:: ------------------------------------------------------------------------------
::    VCopy   ::  robocopy wrapper for qmake
::.     
:: ------------------------------------------------------------------------------
:: :: Usage :: as robocopy (see below)
:: ::
:: :: Return codes :
:: ::
::                   0 - OK  
::                   robocopy errorcode - If robocopy finished with error 
::                                        (see http://ss64.com/nt/robocopy.html)

@echo off

:checkargs
if "%1" == "/?" goto usage

:run
robocopy %*

if %ERRORLEVEL% EQU 16 echo ***FATAL ERROR*** & goto exit_error
if %ERRORLEVEL% EQU 15 echo OKCOPY + FAIL + MISMATCHES + XTRA & goto exit_error
if %ERRORLEVEL% EQU 14 echo FAIL + MISMATCHES + XTRA & goto exit_error
if %ERRORLEVEL% EQU 13 echo OKCOPY + FAIL + MISMATCHES & goto exit_error
if %ERRORLEVEL% EQU 12 echo FAIL + MISMATCHES& goto exit_error
if %ERRORLEVEL% EQU 11 echo OKCOPY + FAIL + XTRA & goto exit_error
if %ERRORLEVEL% EQU 10 echo FAIL + XTRA & goto exit_error
if %ERRORLEVEL% EQU 9 echo OKCOPY + FAIL & goto exit_error
if %ERRORLEVEL% EQU 8 echo FAIL & goto exit_error
if %ERRORLEVEL% EQU 7 echo OKCOPY + MISMATCHES + XTRA & goto exit_success
if %ERRORLEVEL% EQU 6 echo MISMATCHES + XTRA & goto exit_success
if %ERRORLEVEL% EQU 5 echo OKCOPY + MISMATCHES & goto exit_success
if %ERRORLEVEL% EQU 4 echo MISMATCHES & goto exit_success
if %ERRORLEVEL% EQU 3 echo OKCOPY + XTRA & goto exit_success
if %ERRORLEVEL% EQU 2 echo XTRA & goto exit_success
if %ERRORLEVEL% EQU 1 echo OKCOPY & goto exit_success
if %ERRORLEVEL% EQU 0 echo No Change & goto exit_success
goto exit_success

:usage
echo ------------------------------------------------------------------------------
echo    VCopy   ::  robocopy wrapper for qmake
echo.     
echo ------------------------------------------------------------------------------
echo :: Usage :: as robocopy (see below)
echo ::
echo :: Return codes :
echo ::
echo                   0 - OK  
echo                   robocopy errorcode - If robocopy finished with error 
echo                                        (see http://ss64.com/nt/robocopy.html)
robocopy /?
goto exit_success

:exit_error
exit /b %ERRORLEVEL%

:exit_success
exit /b 0