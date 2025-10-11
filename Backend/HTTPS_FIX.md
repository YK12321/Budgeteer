# GitHub Models API HTTPS Fix

## Problem
The error `'https' scheme is not supported` occurred because the application was trying to make HTTPS connections to the GitHub Models API without SSL/TLS support enabled in the cpp-httplib library.

## Root Cause
1. SSL support was explicitly disabled in `LLMInterface.cpp` (line was commented out)
2. The code was using `httplib::Client` instead of `httplib::SSLClient`
3. OpenSSL libraries were not linked in CMakeLists.txt
4. The URL scheme was being passed incorrectly to the HTTP client

## Changes Made

### 1. LLMInterface.cpp
**Enabled SSL Support:**
```cpp
// Before:
// #define CPPHTTPLIB_OPENSSL_SUPPORT

// After:
#define CPPHTTPLIB_OPENSSL_SUPPORT
```

**Changed HTTP Client to SSL Client:**
```cpp
// Before:
httplib::Client cli("https://models.github.ai");

// After:
httplib::SSLClient cli("models.github.ai");
cli.enable_server_certificate_verification(true);
```

**Key changes:**
- Removed `https://` from the hostname (SSLClient handles this automatically)
- Added server certificate verification for security
- Removed misleading warning message about SSL support

### 2. CMakeLists.txt
**Added OpenSSL dependency:**
```cmake
# Find OpenSSL for HTTPS support
find_package(OpenSSL REQUIRED)
```

**Enabled SSL in compiler definitions:**
```cmake
target_compile_definitions(BudgeteerAPI PRIVATE CPPHTTPLIB_OPENSSL_SUPPORT)
```

**Linked OpenSSL libraries:**
```cmake
target_link_libraries(BudgeteerAPI PRIVATE 
    httplib::httplib
    nlohmann_json::nlohmann_json
    OpenSSL::SSL
    OpenSSL::Crypto
)
```

### 3. New Documentation Files

#### OPENSSL_SETUP.md
Comprehensive guide covering:
- Why OpenSSL is needed
- Three installation methods (vcpkg, pre-built binaries, Chocolatey)
- Platform-specific instructions for Windows
- Troubleshooting common issues
- GitHub token setup
- Testing procedures

#### setup-openssl.ps1
Automated PowerShell script that:
- Detects existing OpenSSL installations
- Offers multiple installation methods
- Automatically configures CMake with correct paths
- Builds the project
- Copies necessary DLLs
- Provides next steps

## How to Use

### Quick Setup (Automated)
Run the setup script:
```powershell
.\setup-openssl.ps1
```

### Manual Setup
1. Install OpenSSL (choose one method):
   ```powershell
   # Option 1: Chocolatey (requires admin)
   choco install openssl
   
   # Option 2: vcpkg
   vcpkg install openssl:x64-windows
   
   # Option 3: Download from https://slproweb.com/products/Win32OpenSSL.html
   ```

2. Rebuild the project:
   ```powershell
   cmake -B build -DOPENSSL_ROOT_DIR="C:\Program Files\OpenSSL-Win64"
   cmake --build build --config Debug
   ```

3. Set your GitHub token:
   ```powershell
   $env:GITHUB_TOKEN="ghp_your_token_here"
   ```

4. Run and test:
   ```powershell
   .\build\bin\Debug\BudgeteerAPI.exe
   ```

## Expected Behavior After Fix

### Before (Error):
```
[LLM] Calling GPT-4o-mini via GitHub Models API...
[LLM] Note: Using HTTP client (SSL support requires OpenSSL)
[LLM] Exception calling GitHub API: 'https' scheme is not supported.
[LLM] GPT failed, falling back to local processing
```

### After (Success):
```
[LLM] Calling GPT-4o-mini via GitHub Models API...
[LLM] GPT response received (query 1/1000)
[API] Natural language query: I need fruits
[API] Response: Here are the cheapest options across all stores:
...
```

## Testing the Fix

1. **Verify OpenSSL is found:**
   ```powershell
   cmake -B build 2>&1 | Select-String "OpenSSL"
   ```
   Should show: "Found OpenSSL: ..."

2. **Test API call:**
   ```powershell
   $env:GITHUB_TOKEN="your_token"
   .\build\bin\Debug\BudgeteerAPI.exe
   
   # In another terminal:
   curl -X POST http://localhost:8080/api/llm/query `
     -H "Content-Type: application/json" `
     -d '{"query": "I need fruits"}'
   ```

3. **Check logs:**
   - Should NOT see: "'https' scheme is not supported"
   - Should see: "GPT response received"

## Important Notes

### SSL Certificate Verification
The code now enables certificate verification for security:
```cpp
cli.enable_server_certificate_verification(true);
```

This ensures you're actually connecting to GitHub's servers and not a man-in-the-middle attacker.

### Runtime Dependencies
After building, ensure OpenSSL DLLs are accessible:
- Either in system PATH
- Or copy to `build\bin\Debug\` directory
- The setup script does this automatically

### GitHub Token
Don't forget to set your GitHub Personal Access Token:
```powershell
# Temporary (current session only):
$env:GITHUB_TOKEN="ghp_..."

# Permanent (requires admin):
[System.Environment]::SetEnvironmentVariable('GITHUB_TOKEN', 'ghp_...', [System.EnvironmentVariableTarget]::User)
```

## Troubleshooting

### "Could not find OpenSSL"
Specify the path manually:
```powershell
cmake -B build -DOPENSSL_ROOT_DIR="C:\Program Files\OpenSSL-Win64"
```

### "DLL not found" errors at runtime
Copy OpenSSL DLLs to your build directory:
```powershell
Copy-Item "C:\Program Files\OpenSSL-Win64\bin\*.dll" -Destination "build\bin\Debug\"
```

### Connection still fails
1. Check your GitHub token is valid
2. Verify internet connectivity
3. Check firewall settings
4. Ensure certificate bundle is accessible

## Related Files
- `Backend/src/LLMInterface.cpp` - Main implementation
- `Backend/include/LLMInterface.h` - Header file
- `Backend/CMakeLists.txt` - Build configuration
- `Backend/OPENSSL_SETUP.md` - Detailed setup guide
- `Backend/setup-openssl.ps1` - Automated setup script

## References
- GitHub Models API: https://docs.github.com/en/github-models
- cpp-httplib SSL support: https://github.com/yhirose/cpp-httplib#ssl-support
- OpenSSL for Windows: https://slproweb.com/products/Win32OpenSSL.html
