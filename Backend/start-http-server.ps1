# Start Budgeteer Backend HTTP Server
# This script starts the backend API server in HTTP mode

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Budgeteer Backend HTTP Server" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Check if the executable exists
$exePath = ".\build\bin\Debug\BudgeteerAPI.exe"
if (-not (Test-Path $exePath)) {
    Write-Host "Error: BudgeteerAPI.exe not found!" -ForegroundColor Red
    Write-Host "Please build the project first using:" -ForegroundColor Yellow
    Write-Host "  cd build" -ForegroundColor Yellow
    Write-Host "  cmake --build . --config Debug" -ForegroundColor Yellow
    Write-Host ""
    exit 1
}

Write-Host "Starting HTTP server on port 8080..." -ForegroundColor Green
Write-Host "Backend will be accessible at: http://localhost:8080" -ForegroundColor Green
Write-Host "Press Ctrl+C to stop the server" -ForegroundColor Yellow
Write-Host ""

# Start the server in HTTP mode
& $exePath --http
