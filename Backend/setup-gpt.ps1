# Budgeteer GPT-4o-mini Setup Script
# Run this script to set up and build the project with GitHub Models API

Write-Host "=== Budgeteer GPT-4o-mini (via GitHub) Setup ===" -ForegroundColor Cyan
Write-Host ""

# Step 1: Check for GitHub Token
Write-Host "Step 1: Checking for GitHub Token..." -ForegroundColor Yellow
if ($env:GITHUB_TOKEN) {
    Write-Host "✓ GitHub Token found in environment" -ForegroundColor Green
} else {
    Write-Host "⚠ No GitHub token found" -ForegroundColor Red
    Write-Host ""
    Write-Host "To use GPT-4o-mini features (FREE), set your GitHub token:" -ForegroundColor White
    Write-Host '  $env:GITHUB_TOKEN="ghp_your-token-here"' -ForegroundColor Gray
    Write-Host ""
    Write-Host "Get a token from: https://github.com/settings/tokens" -ForegroundColor Gray
    Write-Host "Required scope: read:packages" -ForegroundColor Gray
    Write-Host ""
    $response = Read-Host "Continue without GPT? (Y/n)"
    if ($response -eq "n" -or $response -eq "N") {
        Write-Host "Setup cancelled. Set API key and run again." -ForegroundColor Yellow
        exit
    }
    Write-Host "✓ Continuing in local-only mode" -ForegroundColor Yellow
}
Write-Host ""

# Step 2: Check Prerequisites
Write-Host "Step 2: Checking prerequisites..." -ForegroundColor Yellow

# Check CMake
try {
    $cmakeVersion = cmake --version 2>&1 | Select-Object -First 1
    Write-Host "✓ CMake: $cmakeVersion" -ForegroundColor Green
} catch {
    Write-Host "✗ CMake not found. Install from: https://cmake.org/download/" -ForegroundColor Red
    exit
}

# Check for compiler
try {
    $clVersion = cl.exe 2>&1 | Select-String "Microsoft"
    if ($clVersion) {
        Write-Host "✓ MSVC compiler found" -ForegroundColor Green
    }
} catch {
    Write-Host "⚠ MSVC not found. Make sure Visual Studio is installed." -ForegroundColor Yellow
    Write-Host "  Or run this from 'Developer PowerShell for VS'" -ForegroundColor Yellow
}

Write-Host ""

# Step 3: Clean previous build
Write-Host "Step 3: Cleaning previous build..." -ForegroundColor Yellow
$buildDir = Join-Path $PSScriptRoot "build"
if (Test-Path $buildDir) {
    Write-Host "Removing old build directory..." -ForegroundColor Gray
    Remove-Item -Recurse -Force $buildDir -ErrorAction SilentlyContinue
}
Write-Host "✓ Clean" -ForegroundColor Green
Write-Host ""

# Step 4: Create build directory
Write-Host "Step 4: Creating build directory..." -ForegroundColor Yellow
New-Item -ItemType Directory -Path $buildDir -Force | Out-Null
Write-Host "✓ Created" -ForegroundColor Green
Write-Host ""

# Step 5: Configure with CMake
Write-Host "Step 5: Configuring with CMake..." -ForegroundColor Yellow
Write-Host "Note: First run will download cpp-httplib and nlohmann/json" -ForegroundColor Gray
Write-Host "This requires internet connection and may take a few minutes..." -ForegroundColor Gray
Write-Host ""

Push-Location $buildDir
try {
    cmake .. 2>&1 | Out-String | Write-Host
    if ($LASTEXITCODE -eq 0) {
        Write-Host "✓ Configuration successful" -ForegroundColor Green
    } else {
        Write-Host "✗ Configuration failed" -ForegroundColor Red
        Pop-Location
        exit
    }
} catch {
    Write-Host "✗ Configuration error: $_" -ForegroundColor Red
    Pop-Location
    exit
}
Write-Host ""

# Step 6: Build
Write-Host "Step 6: Building project..." -ForegroundColor Yellow
try {
    cmake --build . --config Debug 2>&1 | Out-String | Write-Host
    if ($LASTEXITCODE -eq 0) {
        Write-Host "✓ Build successful" -ForegroundColor Green
    } else {
        Write-Host "✗ Build failed" -ForegroundColor Red
        Pop-Location
        exit
    }
} catch {
    Write-Host "✗ Build error: $_" -ForegroundColor Red
    Pop-Location
    exit
}
Pop-Location
Write-Host ""

# Step 7: Verify executable
Write-Host "Step 7: Verifying executable..." -ForegroundColor Yellow
$exePath = Join-Path $buildDir "bin\Debug\BudgeteerAPI.exe"
if (Test-Path $exePath) {
    Write-Host "✓ Executable created: $exePath" -ForegroundColor Green
} else {
    Write-Host "✗ Executable not found" -ForegroundColor Red
    exit
}
Write-Host ""

# Success!
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "✓ Setup Complete!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Your Budgeteer API is ready with GPT-4o-mini via GitHub!" -ForegroundColor White
Write-Host ""

if ($env:GITHUB_TOKEN) {
    Write-Host "GPT Mode: ENABLED via GitHub ✓" -ForegroundColor Green
    Write-Host "Model: openai/gpt-4o-mini (FREE during beta)" -ForegroundColor Gray
} else {
    Write-Host "GPT Mode: LOCAL ONLY" -ForegroundColor Yellow
    Write-Host "Set GITHUB_TOKEN to enable GPT features (FREE!)" -ForegroundColor Gray
}

Write-Host ""
Write-Host "To run the API:" -ForegroundColor White
Write-Host "  cd build\bin\Debug" -ForegroundColor Gray
Write-Host "  .\BudgeteerAPI.exe" -ForegroundColor Gray
Write-Host ""
Write-Host "Documentation:" -ForegroundColor White
Write-Host "  GITHUB_API_GUIDE.md - GitHub Models setup" -ForegroundColor Gray
Write-Host "  Backend\GPT_QUICK_REF.md - Quick reference" -ForegroundColor Gray
Write-Host "  GPT_CHANGES.md - What changed" -ForegroundColor Gray
Write-Host ""
Write-Host "Note: GitHub Models API is FREE during beta! 🎉" -ForegroundColor Cyan
Write-Host ""

$response = Read-Host "Run the API now? (Y/n)"
if ($response -ne "n" -and $response -ne "N") {
    Write-Host ""
    Write-Host "Starting BudgeteerAPI..." -ForegroundColor Cyan
    Push-Location (Join-Path $buildDir "bin\Debug")
    .\BudgeteerAPI.exe
    Pop-Location
}
