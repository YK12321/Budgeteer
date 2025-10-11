# CORS Issue Fix - January 2025

## Problem
The frontend was unable to make POST requests to the backend AI search endpoint due to a CORS error:

```
The 'Access-Control-Allow-Origin' header contains multiple values '*, *', but only one is allowed.
```

## Root Cause
The CORS headers were being set **twice** in the backend:

1. **First time**: In `svr.set_default_headers()` - applies to ALL responses
2. **Second time**: In the `OPTIONS` handler - applies to preflight responses

This caused the header to be duplicated: `Access-Control-Allow-Origin: *, *`

## Solution
Removed the duplicate CORS header setting from the OPTIONS handler.

### Before (ApiServer.cpp)
```cpp
svr.set_default_headers({
    {"Access-Control-Allow-Origin", "*"},
    {"Access-Control-Allow-Methods", "GET, POST, OPTIONS"},
    {"Access-Control-Allow-Headers", "Content-Type"}
});

svr.Options("/(.*)", [](const httplib::Request&, httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");  // ❌ DUPLICATE!
    res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");  // ❌ DUPLICATE!
    res.set_header("Access-Control-Allow-Headers", "Content-Type");  // ❌ DUPLICATE!
    res.status = 200;
});
```

### After (Fixed)
```cpp
svr.set_default_headers({
    {"Access-Control-Allow-Origin", "*"},
    {"Access-Control-Allow-Methods", "GET, POST, OPTIONS"},
    {"Access-Control-Allow-Headers", "Content-Type"}
});

svr.Options("/(.*)", [](const httplib::Request&, httplib::Response& res) {
    res.status = 200;  // ✅ Headers already set by set_default_headers
});
```

## How CORS Works

### Preflight Request (OPTIONS)
Browser sends an OPTIONS request before the actual POST:
```
OPTIONS /api/llm/query HTTP/1.1
Origin: null
Access-Control-Request-Method: POST
Access-Control-Request-Headers: content-type
```

### Server Response
Server must respond with appropriate CORS headers:
```
HTTP/1.1 200 OK
Access-Control-Allow-Origin: *
Access-Control-Allow-Methods: GET, POST, OPTIONS
Access-Control-Allow-Headers: Content-Type
```

### Actual Request (POST)
If preflight passes, browser sends the actual POST request:
```
POST /api/llm/query HTTP/1.1
Content-Type: application/json
{"query": "I need party snacks"}
```

## Testing

### Start Backend
```powershell
$env:GITHUB_TOKEN="your_token"
cd Backend
.\build\bin\Debug\BudgeteerAPI.exe --http
```

### Test with Frontend
1. Open `Frontend/index.html` in browser
2. Click "AI-Powered" search mode
3. Enter a query like "I need party snacks"
4. Click "Search with AI"
5. Should see results without CORS errors

### Test with PowerShell
```powershell
$body = @{ query = "I need party snacks" } | ConvertTo-Json

Invoke-WebRequest -Uri "http://localhost:8080/api/llm/query" `
    -Method POST `
    -ContentType "application/json" `
    -Body $body
```

## Additional Improvements Made

### Frontend Error Handling (app-enhanced.js)
Added detailed console logging for debugging:
```javascript
console.log('[AI Search] Sending query:', query);
console.log('[AI Search] Response status:', response.status);
console.log('[AI Search] Response data:', data);
```

Improved error messages:
```javascript
if (error.message.includes('Failed to fetch')) {
    errorMessage += 'Cannot connect to backend server.\n';
    errorMessage += 'Make sure the backend is running on http://localhost:8080';
}
```

## Files Modified

1. **Backend/src/ApiServer.cpp**
   - Removed duplicate CORS headers from OPTIONS handler

2. **Frontend/app-enhanced.js**
   - Added console logging for debugging
   - Improved error messages

## Verification

✅ CORS preflight request returns 200
✅ CORS headers set correctly (single value)
✅ POST requests work from browser
✅ AI search returns results
✅ No console errors

## Common CORS Mistakes to Avoid

1. ❌ **Setting headers multiple times** (this was our issue)
2. ❌ **Missing OPTIONS handler** (we have one)
3. ❌ **Wrong Access-Control-Allow-Headers** (ours is correct)
4. ❌ **Not handling preflight** (we handle it)
5. ❌ **Forgetting to set status 200 for OPTIONS** (we set it)

## Best Practices

✅ Use `set_default_headers()` for consistent CORS headers
✅ Only set `res.status = 200` in OPTIONS handler
✅ Don't duplicate header settings
✅ Test with browser DevTools Network tab
✅ Check both preflight and actual requests

## Browser Console Test Results

### Before Fix
```
❌ Access to fetch blocked by CORS policy
❌ Multiple values '*, *'
❌ Failed to fetch
```

### After Fix
```
✅ [AI Search] Sending query: I need party snacks
✅ [AI Search] Response status: 200
✅ [AI Search] Response data: {...}
✅ Results displayed
```

## Key Takeaway

**Never set the same headers in both `set_default_headers()` and individual handlers!**

The `set_default_headers()` function applies headers to **all** responses automatically, including OPTIONS, GET, POST, etc. Additional manual `set_header()` calls will **append** to existing headers, not replace them.

---

**Fixed by: GitHub Copilot**
**Date: January 2025**
**Status: ✅ Resolved**
