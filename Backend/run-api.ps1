# Run Budgeteer API with GitHub Token
# This script ensures the GITHUB_TOKEN is set before running the API

Write-Host "=== Budgeteer API - GitHub Token Setup ===" -ForegroundColor Cyan
Write-Host ""

# Check if token is already set and valid
$tokenSet = $false
$currentToken = $env:GITHUB_TOKEN

if ($currentToken -and $currentToken.Length -gt 0) {
    Write-Host "[✓] GITHUB_TOKEN is set (length: $($currentToken.Length) chars)" -ForegroundColor Green
    
    # Verify it looks like a valid token
    if ($currentToken -match '^(ghp_|github_pat_)') {
        Write-Host "[✓] Token format looks valid" -ForegroundColor Green
        $tokenSet = $true
    } else {
        Write-Host "[!] Warning: Token doesn't match expected format (ghp_* or github_pat_*)" -ForegroundColor Yellow
        Write-Host "    Your token: $($currentToken.Substring(0, [Math]::Min(20, $currentToken.Length)))..." -ForegroundColor Gray
    }
} else {
    Write-Host "[✗] GITHUB_TOKEN is not set or is empty" -ForegroundColor Red
}

Write-Host ""

# If token is not set, prompt for it
if (-not $tokenSet) {
    Write-Host "GitHub Personal Access Token is required for GPT-4o-mini features." -ForegroundColor Yellow
    Write-Host ""
    Write-Host "How to get a token:" -ForegroundColor White
    Write-Host "  1. Go to: https://github.com/settings/tokens" -ForegroundColor Gray
    Write-Host "  2. Click 'Generate new token' -> 'Generate new token (classic)'" -ForegroundColor Gray
    Write-Host "  3. Give it a name (e.g., 'Budgeteer API')" -ForegroundColor Gray
    Write-Host "  4. No specific scopes needed for GitHub Models" -ForegroundColor Gray
    Write-Host "  5. Click 'Generate token' and copy it" -ForegroundColor Gray
    Write-Host ""
    
    $choice = Read-Host "Do you want to enter your token now? (Y/n)"
    
    if ($choice -eq "" -or $choice -eq "y" -or $choice -eq "Y") {
        Write-Host ""
        Write-Host "Enter your GitHub token (it will be hidden):" -ForegroundColor Yellow
        $secureToken = Read-Host -AsSecureString
        $BSTR = [System.Runtime.InteropServices.Marshal]::SecureStringToBSTR($secureToken)
        $token = [System.Runtime.InteropServices.Marshal]::PtrToStringAuto($BSTR)
        
        if ($token -and $token.Length -gt 0) {
            $env:GITHUB_TOKEN = $token
            Write-Host "[✓] Token set for this session (length: $($token.Length) chars)" -ForegroundColor Green
            $tokenSet = $true
            
            # Ask if they want to save it permanently
            Write-Host ""
            $save = Read-Host "Save token permanently to user environment? (y/N)"
            if ($save -eq "y" -or $save -eq "Y") {
                [System.Environment]::SetEnvironmentVariable('GITHUB_TOKEN', $token, [System.EnvironmentVariableTarget]::User)
                Write-Host "[✓] Token saved to user environment (permanent)" -ForegroundColor Green
                Write-Host "[i] Note: Restart PowerShell for new sessions to see it" -ForegroundColor Cyan
            }
        } else {
            Write-Host "[✗] No token entered" -ForegroundColor Red
        }
    } else {
        Write-Host ""
        Write-Host "[i] You can set it manually with:" -ForegroundColor Cyan
        Write-Host '    $env:GITHUB_TOKEN="ghp_your_token_here"' -ForegroundColor Gray
        Write-Host ""
        Write-Host "Or set it permanently with:" -ForegroundColor Cyan
        Write-Host '    [System.Environment]::SetEnvironmentVariable("GITHUB_TOKEN", "ghp_your_token_here", [System.EnvironmentVariableTarget]::User)' -ForegroundColor Gray
    }
}

Write-Host ""
Write-Host "=== Running Budgeteer API ===" -ForegroundColor Cyan
Write-Host ""

if (-not $tokenSet) {
    Write-Host "[!] WARNING: Running without GitHub token - GPT features will be disabled" -ForegroundColor Yellow
    Write-Host "[i] The API will work but will use local processing only" -ForegroundColor Yellow
    Write-Host ""
    $continue = Read-Host "Continue anyway? (y/N)"
    if ($continue -ne "y" -and $continue -ne "Y") {
        Write-Host "Cancelled." -ForegroundColor Yellow
        exit
    }
}

# Navigate to the build directory
$buildDir = Join-Path $PSScriptRoot "build\bin\Debug"
if (-not (Test-Path $buildDir)) {
    Write-Host "[✗] Build directory not found: $buildDir" -ForegroundColor Red
    Write-Host "[i] Run the build first:" -ForegroundColor Yellow
    Write-Host "    cmake --build build --config Debug" -ForegroundColor Gray
    exit 1
}

$exePath = Join-Path $buildDir "BudgeteerAPI.exe"
if (-not (Test-Path $exePath)) {
    Write-Host "[✗] Executable not found: $exePath" -ForegroundColor Red
    Write-Host "[i] Run the build first:" -ForegroundColor Yellow
    Write-Host "    cmake --build build --config Debug" -ForegroundColor Gray
    exit 1
}

# Show configuration
Write-Host "Configuration:" -ForegroundColor Cyan
Write-Host "  Executable: $exePath" -ForegroundColor Gray
Write-Host "  GitHub Token: $(if ($env:GITHUB_TOKEN -and $env:GITHUB_TOKEN.Length -gt 0) { "Set ($($env:GITHUB_TOKEN.Length) chars)" } else { "Not set" })" -ForegroundColor Gray
Write-Host "  GPT Mode: $(if ($tokenSet) { "ENABLED" } else { "LOCAL ONLY" })" -ForegroundColor $(if ($tokenSet) { "Green" } else { "Yellow" })
Write-Host ""

# Run the API
Write-Host "Starting API server..." -ForegroundColor Green
Write-Host "Press Ctrl+C to stop" -ForegroundColor Gray
Write-Host ""

Push-Location $buildDir
try {
    & ".\BudgeteerAPI.exe"
} finally {
    Pop-Location
}
