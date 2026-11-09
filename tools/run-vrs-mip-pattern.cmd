@echo off
setlocal
cd /d "%~dp0"
echo PanVK G720 VRS spatially varying mip-content matrix
echo Tests implicit texture() LOD, per-level identity, XY texture gradients, and VRS fragment footprints.
"..\panvk-vrs-render-probe.exe" --case=mip-pattern
set "RESULT=%ERRORLEVEL%"
if exist panvk_vrs_render_probe.log type panvk_vrs_render_probe.log
echo.
echo Probe exit code: %RESULT%
pause
exit /b %RESULT%
