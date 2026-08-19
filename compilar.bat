@echo off
setlocal EnableExtensions EnableDelayedExpansion
cd /d "%~dp0"
if /I "%1"=="doctor" goto doctor
if /I "%1"=="clean" goto clean
if /I "%1"=="rebuild" goto rebuild
if /I "%1"=="run" goto run
where qmake >nul 2>&1 || (echo [ERRO] qmake nao encontrado no PATH.& exit /b 1)
where g++ >nul 2>&1 || (echo [ERRO] g++ nao encontrado no PATH.& exit /b 1)
where mingw32-make >nul 2>&1 || (echo [ERRO] mingw32-make nao encontrado no PATH.& exit /b 1)
for /f "delims=" %%A in ('qmake -query QT_VERSION 2^>nul') do set QT_VERSION=%%A
for /f "delims=" %%A in ('qmake --version 2^>nul ^| findstr /I "QMake"') do set QMAKE_VERSION=%%A
for /f "delims=" %%A in ('g++ --version 2^>nul ^| findstr /I "g++"') do set GXX_VERSION=%%A
echo [OK] !QMAKE_VERSION!
echo [OK] Qt !QT_VERSION!
echo [OK] !GXX_VERSION!
if not exist build mkdir build
cd build
qmake ..\LemonadeAI.pro CONFIG+=release || exit /b 1
mingw32-make -j%NUMBER_OF_PROCESSORS% || exit /b 1
echo BUILD CONCLUIDO: %cd%\release\LemonadeAI.exe
exit /b 0
:doctor
where qmake && qmake -query QT_VERSION && qmake --version
where g++ && g++ --version
where mingw32-make
exit /b 0
:clean
if exist build rmdir /s /q build
echo Build limpo.
exit /b 0
:rebuild
call "%~f0" clean
call "%~f0"
exit /b %errorlevel%
:run
call "%~f0"
if errorlevel 1 exit /b 1
start "" "build\release\LemonadeAI.exe"
exit /b 0
