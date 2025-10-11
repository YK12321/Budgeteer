# OpenSSL Setup for GitHub Models API

## Why OpenSSL is Needed

The GitHub Models API uses HTTPS (secure HTTP), which requires SSL/TLS support. The cpp-httplib library needs OpenSSL to make HTTPS connections.

## Installation on Windows

### Option 1: Using vcpkg (Recommended for Visual Studio)

1. Install vcpkg if you haven't already:
```powershell
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg integrate install
```

2. Install OpenSSL:
```powershell
.\vcpkg install openssl:x64-windows
```

3. Configure CMake to use vcpkg (add to your CMake command):
```powershell
cmake -B build -DCMAKE_TOOLCHAIN_FILE="C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake"
```

### Option 2: Using Pre-built Binaries

1. Download OpenSSL from: https://slproweb.com/products/Win32OpenSSL.html
   - For 64-bit: Download "Win64 OpenSSL v3.x.x" (not the "Light" version)

2. Install to default location (usually `C:\Program Files\OpenSSL-Win64`)

3. Add OpenSSL to system PATH:
   - Right-click "This PC" → Properties → Advanced System Settings
   - Click "Environment Variables"
   - Under "System Variables", find "Path" and click "Edit"
   - Add: `C:\Program Files\OpenSSL-Win64\bin`

4. Set OPENSSL_ROOT_DIR for CMake:
```powershell
$env:OPENSSL_ROOT_DIR="C:\Program Files\OpenSSL-Win64"
cmake -B build
```

### Option 3: Using Chocolatey

1. Install Chocolatey if you haven't: https://chocolatey.org/install

2. Install OpenSSL:
```powershell
choco install openssl
```

3. Refresh environment variables or restart PowerShell

## Building the Project

After installing OpenSSL:

1. Clean previous build (if any):
```powershell
Remove-Item -Recurse -Force build
```

2. Configure and build:
```powershell
cmake -B build
cmake --build build --config Debug
```

## Verifying Installation

Check if OpenSSL is found by CMake:
```powershell
cmake -B build 2>&1 | Select-String "OpenSSL"
```

You should see messages like:
- "Found OpenSSL: ..." (indicating OpenSSL was found)

## Troubleshooting

### Error: "Could not find OpenSSL"

**Solution 1**: Specify OpenSSL path manually:
```powershell
cmake -B build -DOPENSSL_ROOT_DIR="C:\Program Files\OpenSSL-Win64"
```

**Solution 2**: Install OpenSSL via vcpkg and use the toolchain file

### Error: "The procedure entry point... could not be located in the dynamic link library"

**Solution**: Ensure OpenSSL DLLs are in your PATH or copy them to the build directory:
```powershell
Copy-Item "C:\Program Files\OpenSSL-Win64\bin\*.dll" -Destination "build\bin\Debug\"
```

### SSL Connection Fails at Runtime

1. Make sure your GITHUB_TOKEN environment variable is set:
```powershell
$env:GITHUB_TOKEN="your_github_token_here"
```

2. Test the connection:
```powershell
.\build\bin\Debug\BudgeteerAPI.exe
```

## Testing HTTPS Connection

Once built, test the GitHub API connection:

```powershell
# Set your GitHub token
$env:GITHUB_TOKEN="ghp_your_token_here"

# Run the API
.\build\bin\Debug\BudgeteerAPI.exe

# In another terminal, test with curl:
curl -X POST http://localhost:8080/api/llm/query `
  -H "Content-Type: application/json" `
  -d '{"query": "I need milk"}'
```

You should see successful API calls without the "'https' scheme is not supported" error.

## GitHub Token Setup

Don't forget to set your GitHub Personal Access Token:

1. Create a token at: https://github.com/settings/tokens
   - Select "Fine-grained tokens"
   - Set expiration
   - Repository access: All repositories (or select specific ones)
   - Permissions: No specific permissions needed for GitHub Models

2. Set as environment variable (permanent):
```powershell
[System.Environment]::SetEnvironmentVariable('GITHUB_TOKEN', 'ghp_your_token_here', [System.EnvironmentVariableTarget]::User)
```

3. Or set for current session:
```powershell
$env:GITHUB_TOKEN="ghp_your_token_here"
```
