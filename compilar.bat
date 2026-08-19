@echo off
setlocal EnableExtensions EnableDelayedExpansion
chcp 65001 >nul
cd /d "%~dp0"

echo ============================================
echo             NEXORA BUILD SYSTEM
echo ============================================
where qmake >nul 2>&1 || (echo [ERRO] qmake nao encontrado.& exit /b 1)
where g++ >nul 2>&1 || (echo [ERRO] g++ nao encontrado.& exit /b 1)
for /f "delims=" %%A in ('qmake -query QT_VERSION 2^>nul') do set QT_VERSION=%%A
for /f "delims=" %%A in ('qmake --version 2^>nul ^| findstr /I "QMake"') do set QMAKE_VERSION=%%A
for /f "delims=" %%A in ('g++ --version 2^>nul ^| findstr /I "g++"') do set GXX_VERSION=%%A
echo [OK] !QMAKE_VERSION!
echo [OK] Qt !QT_VERSION!
echo [OK] !GXX_VERSION!

if /I "%~1"=="doctor" exit /b 0
if /I "%~1"=="clean" goto clean
if /I "%~1"=="rebuild" goto rebuild
if /I "%~1"=="run" goto run

goto build
:clean
if exist build rmdir /s /q build
echo [OK] Build limpo.
if /I "%~1"=="clean" exit /b 0
goto build
:rebuild
call "%~f0" clean
call "%~f0"
exit /b %errorlevel%
:build
if not exist build mkdir build
cd build
qmake ..\app\Nexora.pro CONFIG+=release || exit /b 1
where mingw32-make >nul 2>&1 && (mingw32-make -j%NUMBER_OF_PROCESSORS% || exit /b 1) || (gmake -j%NUMBER_OF_PROCESSORS% || exit /b 1)
echo [OK] Nexora compilado.
if /I "%~1"=="run" goto launch
exit /b 0
:run
call "%~f0"
if errorlevel 1 exit /b 1
cd build
goto launch
:launch
if exist Nexora.exe (start "Nexora" Nexora.exe) else if exist release\Nexora.exe (start "Nexora" release\Nexora.exe) else (echo [ERRO] Executavel nao encontrado.& exit /b 1)
exit /b 0
