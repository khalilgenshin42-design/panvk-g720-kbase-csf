@echo off
setlocal
cd /d "%~dp0"
title PanVK Mali-G720 Dynamic VRS Test

echo ==========================================
echo PanVK Mali-G720 - Dynamic KEEP VRS test
echo ==========================================
echo.
if not exist "%~dp0panvk-vrs-render-probe.exe" (
  echo ERROR: panvk-vrs-render-probe.exe is missing.
  echo Keep this CMD file beside the EXE.
  echo.
  pause
  exit /b 1
)

if exist "%~dp0panvk_vrs_render_probe.log" del /q "%~dp0panvk_vrs_render_probe.log"

echo Starting dedicated dynamic-keep probe.
echo This build defaults to dynamic-keep; no arguments are needed.
echo Please wait...
echo.
"%~dp0panvk-vrs-render-probe.exe"
set "PROBE_EXIT=%ERRORLEVEL%"

echo.
echo Probe exit code: %PROBE_EXIT%
echo.
if exist "%~dp0panvk_vrs_render_probe.log" (
  echo =============== TEST LOG ===============
  type "%~dp0panvk_vrs_render_probe.log"
) else (
  echo ERROR: No probe log was created.
)
echo.
echo =============== END ===============
echo Send the displayed log or the file:
echo panvk_vrs_render_probe.log
pause
endlocal
