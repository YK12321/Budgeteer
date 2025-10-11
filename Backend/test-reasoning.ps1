# Test script for AI Reasoning feature
# This script demonstrates the iterative reasoning capability

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Budgeteer AI Reasoning Test Suite    " -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Ensure GITHUB_TOKEN is set
if (-not $env:GITHUB_TOKEN) {
    Write-Host "ERROR: GITHUB_TOKEN environment variable is not set!" -ForegroundColor Red
    Write-Host "Please set it with:" -ForegroundColor Yellow
    Write-Host '  $env:GITHUB_TOKEN="your_token_here"' -ForegroundColor Yellow
    exit 1
}

Write-Host "GitHub Token: Set (length: $($env:GITHUB_TOKEN.Length) chars)" -ForegroundColor Green
Write-Host ""

# Test cases that should trigger reasoning
$testCases = @(
    "I need ingredients to make a cake",
    "I want to prepare a salad",
    "Give me ingredients to bake cookies",
    "I need to cook pasta for dinner"
)

Write-Host "Test Cases:" -ForegroundColor Cyan
for ($i = 0; $i -lt $testCases.Count; $i++) {
    Write-Host "  $($i + 1). $($testCases[$i])" -ForegroundColor White
}
Write-Host ""

Write-Host "These queries will trigger the reasoning feature because they contain" -ForegroundColor Yellow
Write-Host "keywords like: 'ingredients', 'make', 'prepare', 'bake', 'cook'" -ForegroundColor Yellow
Write-Host ""

Write-Host "Expected Behavior:" -ForegroundColor Cyan
Write-Host "1. Initial cherry-picking of relevant items" -ForegroundColor White
Write-Host "2. AI reasoning about completeness" -ForegroundColor White
Write-Host "3. Adding missing essential items (e.g., eggs, flour for cake)" -ForegroundColor White
Write-Host "4. Removing unnecessary items" -ForegroundColor White
Write-Host "5. Iterating up to 3 times until list is complete" -ForegroundColor White
Write-Host ""

Write-Host "To test manually:" -ForegroundColor Cyan
Write-Host "  1. Run: .\build\bin\Debug\BudgeteerAPI.exe" -ForegroundColor White
Write-Host "  2. Select option 13 (Natural language query)" -ForegroundColor White
Write-Host "  3. Enter one of the test queries above" -ForegroundColor White
Write-Host "  4. Observe the reasoning logs:" -ForegroundColor White
Write-Host "     - [LLM] Query requires logical reasoning - refining list..." -ForegroundColor Gray
Write-Host "     - [LLM] Reasoning iteration X/3" -ForegroundColor Gray
Write-Host "     - [LLM] + Added: [item]" -ForegroundColor Gray
Write-Host "     - [LLM] Refinement complete" -ForegroundColor Gray
Write-Host ""

Write-Host "Example Output Structure:" -ForegroundColor Cyan
Write-Host @"
[LLM] Found 1500 items, cherry-picking relevant ones...
[LLM] Cherry-picked 8 relevant items
[LLM] Query requires logical reasoning - refining list...
[LLM] Starting reasoning-based refinement (max 3 iterations)...
[LLM] Reasoning iteration 1/3
[LLM] Reasoning: Missing essential baking items like eggs, butter
[LLM] Searching for 2 missing items...
[LLM]   + Added: Eggs (dozen)
[LLM]   + Added: Butter (454g)
[LLM] Reasoning iteration 2/3
[LLM] List is complete after 2 iteration(s)
[LLM] Refinement complete. Final list has 10 unique items
"@ -ForegroundColor Gray

Write-Host ""
Write-Host "Press any key to start the API server for manual testing..." -ForegroundColor Yellow
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")

Write-Host ""
Write-Host "Starting BudgeteerAPI..." -ForegroundColor Cyan
Write-Host ""

# Start the API
& ".\build\bin\Debug\BudgeteerAPI.exe"
