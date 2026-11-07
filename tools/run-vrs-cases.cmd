@echo off
setlocal
cd /d "%~dp0"
echo PanVK VRS isolated render tests
echo Each combiner runs in a separate process so one device-lost result cannot skip the others.
for %%C in (keep max min mul replace) do (
  echo.
  echo ===== Running --case=%%C =====
  "%~dp0panvk-vrs-render-probe.exe" --case=%%C
  if exist panvk_vrs_render_probe.log copy /Y panvk_vrs_render_probe.log panvk_vrs_%%C.log >nul
)
echo.
echo Finished. Send panvk_vrs_keep.log, panvk_vrs_max.log, panvk_vrs_min.log, panvk_vrs_mul.log, panvk_vrs_replace.log and the matching Winlator log.
endlocal
