# Open Budgeteer Frontend
# This script opens the frontend in your default browser

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Budgeteer Frontend" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Check if backend is running
try {
    $response = Invoke-WebRequest -Uri "http://localhost:8080/" -TimeoutSec 2 -ErrorAction Stop
    Write-Host "✓ Backend server is running!" -ForegroundColor Green
    Write-Host "✓ Connected to: http://localhost:8080" -ForegroundColor Green
} catch {
    Write-Host "⚠ Warning: Backend server not detected!" -ForegroundColor Yellow
    Write-Host "  Make sure to start the backend first:" -ForegroundColor Yellow
    Write-Host "  cd ../Backend" -ForegroundColor Yellow
    Write-Host "  .\start-http-server.ps1" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "  Frontend will use mock data for now." -ForegroundColor Yellow
}

Write-Host ""
Write-Host "Opening frontend in your default browser..." -ForegroundColor Green
Write-Host ""

# Open index.html in default browser
Start-Process "index.html"

Write-Host "✓ Frontend opened!" -ForegroundColor Green
Write-Host ""
Write-Host "Note: For best experience, use a local server:" -ForegroundColor Yellow
Write-Host "  python -m http.server 5500" -ForegroundColor Gray
Write-Host "  or use VS Code Live Server extension" -ForegroundColor Gray
Write-Host ""
