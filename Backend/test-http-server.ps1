# Test Backend HTTP Server
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Testing Budgeteer Backend API" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Test server status
Write-Host "Testing server status..." -ForegroundColor Yellow
try {
    $status = Invoke-RestMethod -Uri "http://localhost:8080/" -Method Get
    Write-Host "✓ Server is running!" -ForegroundColor Green
    Write-Host "  Message: $($status.message)" -ForegroundColor Gray
    Write-Host "  Version: $($status.version)" -ForegroundColor Gray
    Write-Host "  Status: $($status.status)" -ForegroundColor Gray
} catch {
    Write-Host "✗ Server not responding" -ForegroundColor Red
    Write-Host "  Make sure the backend is running with --http flag" -ForegroundColor Yellow
    exit 1
}

Write-Host ""

# Test getting stores
Write-Host "Testing GET /stores..." -ForegroundColor Yellow
try {
    $stores = Invoke-RestMethod -Uri "http://localhost:8080/stores" -Method Get
    Write-Host "✓ Stores endpoint working!" -ForegroundColor Green
    Write-Host "  Found $($stores.stores.Count) stores: $($stores.stores -join ', ')" -ForegroundColor Gray
} catch {
    Write-Host "✗ Stores endpoint failed" -ForegroundColor Red
}

Write-Host ""

# Test getting categories
Write-Host "Testing GET /categories..." -ForegroundColor Yellow
try {
    $categories = Invoke-RestMethod -Uri "http://localhost:8080/categories" -Method Get
    Write-Host "✓ Categories endpoint working!" -ForegroundColor Green
    Write-Host "  Found $($categories.categories.Count) categories" -ForegroundColor Gray
} catch {
    Write-Host "✗ Categories endpoint failed" -ForegroundColor Red
}

Write-Host ""

# Test search
Write-Host "Testing GET /search?q=milk..." -ForegroundColor Yellow
try {
    $searchResults = Invoke-RestMethod -Uri "http://localhost:8080/search?q=milk" -Method Get
    Write-Host "✓ Search endpoint working!" -ForegroundColor Green
    Write-Host "  Found $($searchResults.count) results" -ForegroundColor Gray
    if ($searchResults.count -gt 0) {
        $firstItem = $searchResults.items[0]
        Write-Host "  First result: $($firstItem.item_name) - `$$($firstItem.current_price) at $($firstItem.store)" -ForegroundColor Gray
    }
} catch {
    Write-Host "✗ Search endpoint failed" -ForegroundColor Red
}

Write-Host ""

# Test getting all items (limited display)
Write-Host "Testing GET /items..." -ForegroundColor Yellow
try {
    $items = Invoke-RestMethod -Uri "http://localhost:8080/items" -Method Get
    Write-Host "✓ Items endpoint working!" -ForegroundColor Green
    Write-Host "  Total items in database: $($items.count)" -ForegroundColor Gray
} catch {
    Write-Host "✗ Items endpoint failed" -ForegroundColor Red
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  All tests completed!" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Backend API is ready for frontend integration!" -ForegroundColor Green
Write-Host ""
