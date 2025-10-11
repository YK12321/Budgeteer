# GitHub Models API Content-Type Fix

## Problem
After fixing the HTTPS/SSL issue, a new error appeared:
```
[LLM] GitHub API Error: 400 - Content-Type must be application/json
```

## Root Cause
The cpp-httplib `Post()` method was being called with conflicting Content-Type specifications:
1. Content-Type in the `headers` map
2. Content-Type as the 4th parameter (`"application/json"`)

When both are specified, the library might send duplicate or malformed headers, causing GitHub's API to reject the request with a 400 error.

## Solution
Remove `Content-Type` from the headers map and let cpp-httplib set it via the content type parameter:

### Before (Incorrect):
```cpp
httplib::Headers headers = {
    {"Content-Type", "application/json"},  // ❌ Conflicting
    {"Authorization", "Bearer " + openaiApiKey}
};

auto res = cli.Post("/inference/chat/completions", headers, body, "application/json");
                                                                  // ❌ Duplicate Content-Type
```

### After (Correct):
```cpp
httplib::Headers headers = {
    {"Authorization", "Bearer " + openaiApiKey}  // ✅ Only auth header
};

auto res = cli.Post("/inference/chat/completions", headers, body, "application/json");
                                                                  // ✅ Library sets Content-Type
```

## How cpp-httplib Works

The `Post()` method signature:
```cpp
Result Post(const std::string &path, 
            const Headers &headers,
            const std::string &body, 
            const std::string &content_type)
```

When you provide the `content_type` parameter, cpp-httplib automatically adds the `Content-Type` header. If you also include it in `headers`, you may get:
- Duplicate headers
- Incorrect header formatting
- API rejection (400 Bad Request)

## Testing the Fix

1. Rebuild the project:
   ```powershell
   cmake --build build --config Debug
   ```

2. Ensure your GitHub token is set:
   ```powershell
   $env:GITHUB_TOKEN="ghp_your_token_here"
   ```

3. Run the API:
   ```powershell
   .\build\bin\Debug\BudgeteerAPI.exe
   ```

4. Test with a query:
   ```powershell
   # In another terminal:
   curl -X POST http://localhost:8080/api/llm/query `
     -H "Content-Type: application/json" `
     -d '{"query": "I need to buy fruits"}'
   ```

## Expected Output (After Fix)

### Before (Error):
```
[LLM] Calling GPT-4o-mini via GitHub Models API...
[LLM] GitHub API Error: 400 - Content-Type must be application/json
[LLM] GPT failed, falling back to local processing
```

### After (Success):
```
[LLM] Calling GPT-4o-mini via GitHub Models API...
[LLM] GPT response received (query 1/1000)

Here are the cheapest options across all stores:
| Store     | Item                          | Price   | Notes              |
|-----------|-------------------------------|---------|-----------------------|
| Walmart   | Fresh Apples 3lb              | $4.99   | In stock           |
| Loblaws   | Bananas per lb                | $0.69   | In stock           |
...
```

## Additional Notes

### Proper Header Usage in cpp-httplib

**DO:**
- ✅ Use the `content_type` parameter for Content-Type
- ✅ Put custom headers (like Authorization) in the `headers` map
- ✅ Let the library handle standard HTTP headers when possible

**DON'T:**
- ❌ Duplicate headers between `headers` map and function parameters
- ❌ Manually set Content-Type if using the `content_type` parameter
- ❌ Mix different header-setting methods for the same header

### GitHub Models API Requirements

The GitHub Models API is strict about headers:
- `Authorization: Bearer <token>` - Required, must be valid GitHub token
- `Content-Type: application/json` - Required, must be exactly this value
- Body must be valid JSON matching the API schema

## Related Files
- `Backend/src/LLMInterface.cpp` - Line ~120 (callGPTAPI method)
- `Backend/HTTPS_FIX.md` - Previous SSL/HTTPS fix
- `Backend/OPENSSL_SETUP.md` - OpenSSL setup guide

## Complete Fix Summary

Two issues were fixed to get GitHub Models API working:

1. **HTTPS/SSL Support** (HTTPS_FIX.md)
   - Enabled OpenSSL support
   - Changed to SSLClient
   - Linked OpenSSL libraries

2. **Content-Type Header** (This fix)
   - Removed duplicate Content-Type from headers
   - Let cpp-httplib set it via parameter
   - Fixed 400 Bad Request error

Both fixes are now in place and the GitHub Models API integration should work correctly!
