@echo off
REM Build script for Budgeteer API

echo ========================================
echo    Budgeteer API Build Script
echo ========================================
echo.

REM Check if Make is available
where make >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo Using Make...
    make
) else (
    echo Make not found. Checking for g++...
    where g++ >nul 2>&1
    if %ERRORLEVEL% EQU 0 (
        echo Using g++ directly...
        
        REM Create directories
        if not exist "obj" mkdir obj
        if not exist "bin" mkdir bin
        if not exist "bin\SampleDataset" mkdir bin\SampleDataset
        xcopy /E /I /Y "SampleDataset" "bin\SampleDataset" > nul
        
        REM Compile
        echo Compiling Item.cpp...
        g++ -std=c++17 -Wall -Wextra -Iinclude -c src\Item.cpp -o obj\Item.o
        
        echo Compiling Database.cpp...
        g++ -std=c++17 -Wall -Wextra -Iinclude -c src\Database.cpp -o obj\Database.o
        
        echo Compiling ApiServer.cpp...
        g++ -std=c++17 -Wall -Wextra -Iinclude -c src\ApiServer.cpp -o obj\ApiServer.o
        
        echo Compiling main.cpp...
        g++ -std=c++17 -Wall -Wextra -Iinclude -c src\main.cpp -o obj\main.o
        
        echo Linking...
        g++ -std=c++17 obj\Item.o obj\Database.o obj\ApiServer.o obj\main.o -o bin\BudgeteerAPI.exe
        
        echo.
        echo Build complete! Executable: bin\BudgeteerAPI.exe
    ) else (
        echo ERROR: Neither Make nor g++ found!
        echo Please install MinGW-w64 or MSYS2 to get g++ and make.
        exit /b 1
    )
)

echo.
echo ========================================
echo Build finished successfully!
echo ========================================
echo.
echo To run the application:
echo   cd bin
echo   BudgeteerAPI.exe
echo.
pause
