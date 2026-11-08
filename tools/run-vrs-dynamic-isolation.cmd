@echo off
setlocal
cd /d "%~dp0"
echo PanVK G720 dynamic pipeline FSR constant-colour isolation check
"%~dp0..\panvk-vrs-render-probe.exe" --case=dynamic-isolation
if exist panvk_vrs_render_probe.log (
  copy /Y panvk_vrs_render_probe.log panvk_vrs_dynamic_isolation.log >nul
)
echo.
echo Send panvk_vrs_dynamic_isolation.log only.
endlocal
