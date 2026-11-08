@echo off
setlocal
cd /d "%~dp0"
echo PanVK G720 VRS advertised-rate matrix
echo Tests static and dynamic KEEP for 2x1, 1x2 and 4x4.
"%~dp0..\panvk-vrs-render-probe.exe" --case=rate-matrix
set "RESULT=%ERRORLEVEL%"
if exist panvk_vrs_render_probe.log type panvk_vrs_render_probe.log
echo.
echo Probe exit code: %RESULT%
pause
exit /b %RESULT%
