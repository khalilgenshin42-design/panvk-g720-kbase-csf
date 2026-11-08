@echo off
setlocal
cd /d "%~dp0"
echo PanVK G720 dynamic pipeline FSR render check
"%~dp0..\panvk-vrs-render-probe.exe" --case=dynamic-keep
if exist panvk_vrs_render_probe.log (
  copy /Y panvk_vrs_render_probe.log panvk_vrs_dynamic.log >nul
)
echo.
echo Send panvk_vrs_dynamic.log only; no Winlator logcat archive is required.
endlocal
