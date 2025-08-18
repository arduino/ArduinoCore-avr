@echo off
set ARGS=/LM /SW /SA
if "%PROCESSOR_ARCHITECTURE%" == "AMD64" (
  drivers\dpinst-amd64.exe %ARGS%
) ELSE IF "%PROCESSOR_ARCHITEW6432%" == "AMD64" (
  drivers\dpinst-amd64.exe %ARGS%
) ELSE (
  drivers\dpinst-x86.exe %ARGS%
)

REM dpinst /PATH has problems with relative paths, so use absolute path.
if "%PROCESSOR_ARCHITECTURE%" == "AMD64" (
  drivers\dpinst-amd64.exe /PATH "%cd%\drivers\gemma" %ARGS%
) ELSE IF "%PROCESSOR_ARCHITEW6432%" == "AMD64" (
  drivers\dpinst-amd64.exe /PATH "%cd%\drivers\gemma" %ARGS%
) ELSE (
  drivers\dpinst-x86.exe /PATH "%cd%\drivers\gemma" %ARGS%
)

exit /b 0
