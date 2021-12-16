@echo off
set ARGS=/SE /SW /SA
if "%PROCESSOR_ARCHITECTURE%" == "AMD64" (
  drivers\dpinst-amd64.exe %ARGS%
) ELSE IF "%PROCESSOR_ARCHITEW6432%" == "AMD64" (
  drivers\dpinst-amd64.exe %ARGS%
) ELSE (
  drivers\dpinst-x86.exe %ARGS%
)

@echo off
setlocal
for /f "tokens=4-5 delims=[.] " %%i in ('ver') do @(if %%i==Version (set VERSION=%%j) else (set VERSION=%%i))
if %VERSION% GEQ 10 (
  exit /b 0
)
endlocal

REM dpinst /PATH has problems with relative paths, so use absolute path.
if "%PROCESSOR_ARCHITECTURE%" == "AMD64" (
  drivers\dpinst-amd64.exe /PATH %cd%\drivers\prewin10 %ARGS%
) ELSE IF "%PROCESSOR_ARCHITEW6432%" == "AMD64" (
  drivers\dpinst-amd64.exe /PATH %cd%\drivers\prewin10 %ARGS%
) ELSE (
  drivers\dpinst-x86.exe /PATH %cd%\drivers\prewin10 %ARGS%
)

exit /b 0
