# Quick Reference Guide

## 🚀 Quick Start

### Start Backend
```powershell
cd Backend
$env:GITHUB_TOKEN="YOUR_GITHUB_TOKEN_HERE"
.\build\bin\Debug\BudgeteerAPI.exe
```

### Start Frontend
```powershell
cd Frontend
Start-Process "index.html"
```

---

## 🎨 UI Components

### Search Mode Toggle
```html
Basic Search    AI-Powered
[  Active  ]    [         ]
```

### Mascot
```
    🦆
   Budgie
(Floating animation)
```

### AI Search Card
```
⭐ Ask Budgie Anything!

[                          ]
[   Describe what you need ]
[                          ]

[ ⭐ Search with AI ]
```

---

## 🔌 API Endpoints

### Basic Search
```
GET /api/items/search?query={product}
```

### AI Search
```
POST /api/llm/query
Body: { "query": "natural language request" }
```

### Response Format
```json
{
  "success": true,
  "query": "user query",
  "response": "table with results"
}
```

---

## 💻 Code Snippets

### Toggle Search Mode (JS)
```javascript
basicSearchToggle.addEventListener('click', () => {
    basicSearchToggle.classList.add('active');
    aiSearchToggle.classList.remove('active');
    basicSearchSection.style.display = 'block';
    aiSearchSection.style.display = 'none';
});
```

### AI Search API Call (JS)
```javascript
const response = await fetch(`${API_BASE_URL}/api/llm/query`, {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify({ query: userQuery })
});
```

### Validate Final List (C++)
```cpp
std::vector<Item> LLMInterface::validateFinalList(
    const std::string& query, 
    const std::vector<Item>& items
) {
    // Ask GPT to identify obviously wrong items
    // Remove only clear mistakes
    // Return validated list
}
```

---

## 🎯 Example Queries

### Basic Search
```
Samsung TV
Baby Shampoo
Dish Soap
Milk 2%
Bread
```

### AI Search
```
I need party snacks under $30
Find me ingredients to bake a cake
Looking for affordable breakfast items
Need cleaning supplies for the house
Weekly groceries for family of 4
```

---

## 🎨 CSS Classes

### Mode Toggle
```css
.mode-toggle-btn          /* Base button style */
.mode-toggle-btn.active   /* Active state */
```

### Mascot
```css
.mascot-container         /* Container with animation */
.mascot-image             /* Image styling */
```

### AI Search
```css
.ai-search-section        /* Section wrapper */
.ai-search-card           /* Card container */
.ai-search-input          /* Textarea */
.ai-search-button         /* Action button */
```

---

## 📱 Responsive Breakpoints

```css
@media (max-width: 768px) {
    /* Mobile styles */
    .mascot-image: 80px
    .search-mode-toggle: column
    .mode-toggle-btn: full width
}
```

---

## 🐛 Troubleshooting

### Backend not responding
```powershell
# Check if server is running
netstat -an | findstr "8080"

# Restart server
# Press Ctrl+C in backend terminal
# Run .\build\bin\Debug\BudgeteerAPI.exe again
```

### Frontend shows CORS error
```javascript
// Make sure API_BASE_URL is correct
const API_BASE_URL = 'http://localhost:8080';
```

### AI search returns empty
```
Check:
1. GitHub token is set
2. Backend has internet connection
3. GPT API is not rate limited
```

---

## 🎨 Color Palette

```css
Primary:    #4F46E5  /* Indigo */
Secondary:  #10B981  /* Green */
Error:      #EF4444  /* Red */
Background: #F9FAFB  /* Light Gray */
Text:       #111827  /* Near Black */
```

---

## ⌨️ Keyboard Shortcuts

```
Enter          → Trigger search (in any input)
Shift+Enter    → New line in AI textarea
Esc            → Close modal
Tab            → Navigate between inputs
```

---

## 📊 Performance Tips

### Backend
- Cache validation results
- Limit GPT calls
- Use fallback for errors

### Frontend
- Lazy load images
- Debounce search input
- Use CSS animations (GPU)

---

## 🔐 Security

### API Key
```powershell
# Set in environment
$env:GITHUB_TOKEN="your_token"

# Never commit to git!
# Add to .gitignore
```

### CORS
```cpp
// Backend handles CORS
res.set_header("Access-Control-Allow-Origin", "*");
```

---

## 📝 Testing Checklist

```
Backend:
☐ Build succeeds
☐ Server starts on :8080
☐ Basic search works
☐ AI search works
☐ Validation runs
☐ No console errors

Frontend:
☐ Page loads
☐ Mode toggle works
☐ Basic search functional
☐ AI search functional
☐ Results display
☐ Mobile responsive
☐ Animations smooth
```

---

## 🆘 Quick Fixes

### Problem: AI returns "no items"
**Fix:** Cherry-pick is too strict
```cpp
// Check if GPT returned empty array
// Fall back to top 20 items
```

### Problem: Validation removes all items
**Fix:** Prompt too strict
```cpp
// Update validation prompt
// Make it more lenient
```

### Problem: Search doesn't match
**Fix:** Use partial matching
```cpp
// Use substring matching
// Case-insensitive comparison
```

---

## 📚 Resources

- Backend API: `http://localhost:8080`
- GitHub Models: `https://github.com/marketplace/models`
- Documentation: See `IMPLEMENTATION_SUMMARY.md`
- Visual Guide: See `VISUAL_GUIDE.md`

---

## 🏁 Final Checks

```bash
# Backend running?
curl http://localhost:8080/api/stats

# Frontend loaded?
# Open http://localhost:3000 or file:///.../index.html

# GitHub token set?
echo $env:GITHUB_TOKEN
```

---

**Quick Reference v1.0**
**Last Updated: January 2025**
