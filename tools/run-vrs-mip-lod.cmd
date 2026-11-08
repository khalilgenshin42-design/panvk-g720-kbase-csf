@echo off
setlocal
cd /d "%~dp0"
echo PanVK G720 VRS implicit mip-LOD matrix
echo Tests static and dynamic KEEP using texture() implicit derivatives.
"..\panvk-vrs-render-probe.exe" --case=mip-lod
set "RESULT=%ERRORLEVEL%"
if exist panvk_vrs_render_probe.log type panvk_vrs_render_probe.log
echo.
echo Probe exit code: %RESULT%
pause
exit /b %RESULT%
