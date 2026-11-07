@echo off
setlocal
cd /d "%~dp0"
echo PanVK G720 VRS KEEP-only regression check
"%~dp0..\panvk-vrs-render-probe.exe" --case=keep
if exist panvk_vrs_render_probe.log (
  copy /Y panvk_vrs_render_probe.log panvk_vrs_keep.log >nul
)
echo.
echo Send panvk_vrs_keep.log and the matching Winlator logcat archive.
endlocal
