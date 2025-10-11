# Test Budgeteer API with various queries

Write-Host "=== Budgeteer API Test Script ===" -ForegroundColor Cyan
Write-Host ""

$apiUrl = "http://localhost:8080"

# Function to test API endpoint
function Test-ApiEndpoint {
    param(
        [string]$Endpoint,
        [string]$Method = "GET",
        [object]$Body = $null
    )
    
    try {
        $headers = @{
            "Content-Type" = "application/json"
        }
        
        $params = @{
            Uri = "$apiUrl$Endpoint"
            Method = $Method
            Headers = $headers
            TimeoutSec = 30
        }
        
        if ($Body) {
            $params.Body = ($Body | ConvertTo-Json -Depth 10)
        }
        
        $response = Invoke-RestMethod @params
        return $response
    } catch {
        Write-Host "[✗] Error: $($_.Exception.Message)" -ForegroundColor Red
        if ($_.Exception.Response) {
            $reader = New-Object System.IO.StreamReader($_.Exception.Response.GetResponseStream())
            $responseBody = $reader.ReadToEnd()
            Write-Host "Response: $responseBody" -ForegroundColor Gray
        }
        return $null
    }
}

# Wait for API to be ready
Write-Host "Waiting for API to be ready..." -ForegroundColor Yellow
$maxRetries = 10
$retryCount = 0
$apiReady = $false

while ($retryCount -lt $maxRetries -and -not $apiReady) {
    try {
        $response = Invoke-WebRequest -Uri "$apiUrl/api/health" -Method GET -TimeoutSec 2 -ErrorAction Stop
        if ($response.StatusCode -eq 200) {
            $apiReady = $true
            Write-Host "[✓] API is ready!" -ForegroundColor Green
        }
    } catch {
        $retryCount++
        Write-Host "." -NoNewline
        Start-Sleep -Seconds 1
    }
}

if (-not $apiReady) {
    Write-Host ""
    Write-Host "[✗] API is not responding. Make sure it's running:" -ForegroundColor Red
    Write-Host "    .\run-api.ps1" -ForegroundColor Gray
    exit 1
}

Write-Host ""
Write-Host "=== Running Tests ===" -ForegroundColor Cyan
Write-Host ""

# Test 1: Health check
Write-Host "Test 1: Health Check" -ForegroundColor Yellow
$health = Test-ApiEndpoint -Endpoint "/api/health"
if ($health) {
    Write-Host "[✓] Health check passed" -ForegroundColor Green
    Write-Host "    Response: $($health | ConvertTo-Json -Compress)" -ForegroundColor Gray
} else {
    Write-Host "[✗] Health check failed" -ForegroundColor Red
}
Write-Host ""

# Test 2: Simple product search
Write-Host "Test 2: Simple Product Search (milk)" -ForegroundColor Yellow
$search = Test-ApiEndpoint -Endpoint "/api/search?product=milk"
if ($search) {
    Write-Host "[✓] Search successful" -ForegroundColor Green
    Write-Host "    Found $($search.Count) results" -ForegroundColor Gray
} else {
    Write-Host "[✗] Search failed" -ForegroundColor Red
}
Write-Host ""

# Test 3: Natural language query (local processing)
Write-Host "Test 3: Natural Language Query - Simple (should use local)" -ForegroundColor Yellow
$query1 = @{
    query = "find milk"
}
$llm1 = Test-ApiEndpoint -Endpoint "/api/llm/query" -Method "POST" -Body $query1
if ($llm1) {
    Write-Host "[✓] Query processed" -ForegroundColor Green
    Write-Host "    Response length: $($llm1.response.Length) chars" -ForegroundColor Gray
} else {
    Write-Host "[✗] Query failed" -ForegroundColor Red
}
Write-Host ""

# Test 4: Natural language query (GPT processing)
Write-Host "Test 4: Natural Language Query - Complex (should use GPT if token is set)" -ForegroundColor Yellow
$query2 = @{
    query = "I need to buy fruits for a party, what are my cheapest options?"
}
$llm2 = Test-ApiEndpoint -Endpoint "/api/llm/query" -Method "POST" -Body $query2
if ($llm2) {
    Write-Host "[✓] Query processed" -ForegroundColor Green
    Write-Host "    Response:" -ForegroundColor Gray
    Write-Host "    $($llm2.response)" -ForegroundColor White
} else {
    Write-Host "[✗] Query failed" -ForegroundColor Red
}
Write-Host ""

# Test 5: Store comparison
Write-Host "Test 5: Store Comparison (apples)" -ForegroundColor Yellow
$compare = Test-ApiEndpoint -Endpoint "/api/compare?product=apples"
if ($compare) {
    Write-Host "[✓] Comparison successful" -ForegroundColor Green
    Write-Host "    Found results from multiple stores" -ForegroundColor Gray
} else {
    Write-Host "[✗] Comparison failed" -ForegroundColor Red
}
Write-Host ""

Write-Host "=== Test Summary ===" -ForegroundColor Cyan
Write-Host ""
Write-Host "All tests completed. Check the API output for detailed logs." -ForegroundColor White
Write-Host ""
Write-Host "To test manually:" -ForegroundColor Yellow
Write-Host '  Invoke-RestMethod -Uri "http://localhost:8080/api/llm/query" -Method POST -Headers @{"Content-Type"="application/json"} -Body (\'{"query": "I need fruits"}\' | ConvertTo-Json)' -ForegroundColor Gray
Write-Host ""
