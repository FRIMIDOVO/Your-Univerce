@echo off
cd /d "%~dp0"


if not exist bin mkdir bin

echo Compiling...
echo.

g++ -std=c++20 -O2 ^
    scr\particle.cpp ^
    scr\config_loader.cpp ^
    scr\gif_recorder.cpp ^
    scr\hex_grid.cpp ^
    scr\physics.cpp ^
    scr\physics_laws.cpp ^
    scr\visualizer.cpp ^
    scr\main.cpp ^
    -Iinclude ^
    -I"F:\Libs\SFML-3.1.0\include" ^
    -L"F:\Libs\SFML-3.1.0\lib" ^
    -lsfml-graphics -lsfml-window -lsfml-system ^
    -o bin\universe.exe

if %errorlevel% == 0 (
    echo.
    echo ========================================
    echo   Build successful!
    echo ========================================
    echo.
) else (
    echo.
    echo ========================================
    echo   Build failed!
    echo ========================================
)

pause