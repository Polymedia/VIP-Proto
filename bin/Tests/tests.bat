@echo off

set exitCode=0

setlocal EnableDelayedExpansion

:main
if "%1"=="-h" goto help
if "%1"=="--help" goto help
if "%1"=="-?" goto help

goto run

:help
echo Usage:
echo     tests.bat [file_pattern]
goto exit

:run
set startTime=%TIME%

set failedTests=
set /a total=0
set /a passed=0
set /a failed=0

echo.
echo ************************************************************
echo ********************* TEST SUITE START *********************
echo ************************************************************
echo.

set pattern=%1
rem If no pattern specified, run all executables in directory
if %pattern%.==. set pattern=tst_*.exe

for /f %%f in ('dir /b %pattern%') do (
	%%f
	set /a total=!total!+1
	
	if !errorlevel! NEQ 0 (		
		set failedTests=!failedTests! %%f 
		set /a failed=!failed!+1
	) else (
		set /a passed=!passed!+1
	)
)

echo.
echo ************************************************************
echo ********************** TEST SUITE END **********************
echo ************************************************************

set endTime=%TIME%

rem Change formatting for the start and end times
for /f "tokens=1-4 delims=:.," %%a in ("%startTime%") do (
   set /a "start=(((%%a*60)+1%%b %% 100)*60+1%%c %% 100)*100+1%%d %% 100"
)

for /F "tokens=1-4 delims=:.," %%a in ("%endTime%") do (
   set /a "end=(((%%a*60)+1%%b %% 100)*60+1%%c %% 100)*100+1%%d %% 100"
)

rem Calculate the elapsed time by subtracting values
set /a elapsed=end-start

rem Format the results for output
set /a rest=elapsed%%(60*60*60*100*100), ss=rest/100, cc=rest%%100
if %cc% lss 10 set cc=0%cc%

set duration=%ss%.%cc%0

echo Start time: %startTime%
echo End time:   %endTime%
echo Total seconds elapsed: %duration%s
echo.

echo Total executed: !total!
echo Passed:         !passed!
echo Failed:         !failed!
echo.

if !failed! NEQ 0 (
	echo Folowing tests have failed:
	for %%t in (!failedTests!) do (
		echo     %%t
	)
) else (
	echo OK
)

set exitCode=!failed!

:exit
exit /b %exitCode%
	