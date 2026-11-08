@echo off
setlocal
cd /d "%~dp0"
echo PanVK G720 VRS REPLACE-only combiner check
"%~dp0..\panvk-vrs-render-probe.exe" --case=replace
if exist panvk_vrs_render_probe.log (
  copy /Y panvk_vrs_render_probe.log panvk_vrs_replace.log >nul
)
echo.
echo Send panvk_vrs_replace.log only; no Winlator logcat archive is required.
endlocal
