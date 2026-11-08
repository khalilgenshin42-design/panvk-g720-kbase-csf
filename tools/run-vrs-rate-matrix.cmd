@echo off
setlocal
cd /d "%~dp0"
echo PanVK G720 VRS advertised-rate matrix
echo Tests static and dynamic KEEP for all advertised non-2x2 rates.
"%~dp0..\panvk-vrs-render-probe.exe" --case=rate-matrix
set "RESULT=%ERRORLEVEL%"
if exist panvk_vrs_render_probe.log type panvk_vrs_render_probe.log
echo.
echo Probe exit code: %RESULT%
pause
exit /b %RESULT%
