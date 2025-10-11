# Test script for AI Search API

Write-Host "Testing AI Search API..." -ForegroundColor Cyan
Write-Host ""

# Test data
$body = @{
    query = "I need party snacks"
} | ConvertTo-Json

Write-Host "Request:" -ForegroundColor Yellow
Write-Host $body
Write-Host ""

try {
    # Make request
    Write-Host "Sending POST request to http://localhost:8080/api/llm/query..." -ForegroundColor Yellow
    
    $response = Invoke-WebRequest -Uri "http://localhost:8080/api/llm/query" `
        -Method POST `
        -ContentType "application/json" `
        -Body $body `
        -UseBasicParsing
    
    Write-Host ""
    Write-Host "Response Status: $($response.StatusCode)" -ForegroundColor Green
    Write-Host ""
    Write-Host "Response Body:" -ForegroundColor Yellow
    $response.Content | ConvertFrom-Json | ConvertTo-Json -Depth 10
    
} catch {
    Write-Host ""
    Write-Host "Error: $($_.Exception.Message)" -ForegroundColor Red
    Write-Host ""
    Write-Host "Error Details:" -ForegroundColor Yellow
    Write-Host $_.Exception | Format-List * -Force
}

Write-Host ""
Write-Host "Test complete!" -ForegroundColor Cyan
