# GitHub Actions Workflow Fixes

**Date:** October 11, 2025  
**Issue:** Windows and macOS builds failing in CI/CD pipeline

---

## Problems Identified

### 1. Windows Build Issue ❌

**Error:**
```
Could NOT find OpenSSL, try to set the path to OpenSSL root folder in the
system variable OPENSSL_ROOT_DIR (missing: OPENSSL_CRYPTO_LIBRARY) (found
version "3.6.0")
```

**Root Cause:**
- OpenSSL was installed via Chocolatey successfully
- CMake could find OpenSSL but couldn't locate the crypto library
- `OPENSSL_ROOT_DIR` environment variable was not set for CMake

### 2. macOS Build Issue ❌

**Error:**
```
CMake Error at build/_deps/json-src/CMakeLists.txt:1 (cmake_minimum_required):
  Compatibility with CMake < 3.5 has been removed from CMake.
  Update the VERSION argument <min> value.
```

**Root Cause:**
- The nlohmann/json library v3.11.2 requires CMake 3.5+ minimum
- macOS runner had an older CMake version from brew
- Version conflict between system CMake and library requirements

---

## Solutions Implemented ✅

### 1. Windows Build Fix

**Changes to `.github/workflows/build.yml` and `.github/workflows/release.yml`:**

```yaml
- name: Find OpenSSL
  id: find-openssl
  shell: powershell
  run: |
    # Check multiple possible OpenSSL installation paths
    $possiblePaths = @(
      "C:\Program Files\OpenSSL-Win64",
      "C:\Program Files\OpenSSL",
      "C:\OpenSSL-Win64",
      "C:\OpenSSL"
    )
    
    $opensslPath = $null
    foreach ($path in $possiblePaths) {
      if (Test-Path $path) {
        $opensslPath = $path
        Write-Host "OpenSSL found at: $opensslPath"
        break
      }
    }
    
    if (-not $opensslPath) {
      Write-Error "OpenSSL installation not found"
      exit 1
    }
    
    # Set environment variables for CMake
    echo "OPENSSL_ROOT_DIR=$opensslPath" >> $env:GITHUB_ENV
    
    # Also set individual library paths
    $cryptoLib = "$opensslPath\lib\libcrypto.lib"
    $sslLib = "$opensslPath\lib\libssl.lib"
    
    if (Test-Path $cryptoLib) {
      echo "OPENSSL_CRYPTO_LIBRARY=$cryptoLib" >> $env:GITHUB_ENV
    }
    
    if (Test-Path $sslLib) {
      echo "OPENSSL_SSL_LIBRARY=$sslLib" >> $env:GITHUB_ENV
    }
    
    # Set include directory
    $includeDir = "$opensslPath\include"
    if (Test-Path $includeDir) {
      echo "OPENSSL_INCLUDE_DIR=$includeDir" >> $env:GITHUB_ENV
    }

- name: Configure CMake
  shell: powershell
  run: |
    cd Backend
    $cmakeArgs = @(
      "-B", "build",
      "-DCMAKE_BUILD_TYPE=Release",
      "-DOPENSSL_ROOT_DIR=$env:OPENSSL_ROOT_DIR"
    )
    
    if ($env:OPENSSL_CRYPTO_LIBRARY) {
      $cmakeArgs += "-DOPENSSL_CRYPTO_LIBRARY=$env:OPENSSL_CRYPTO_LIBRARY"
    }
    
    if ($env:OPENSSL_SSL_LIBRARY) {
      $cmakeArgs += "-DOPENSSL_SSL_LIBRARY=$env:OPENSSL_SSL_LIBRARY"
    }
    
    if ($env:OPENSSL_INCLUDE_DIR) {
      $cmakeArgs += "-DOPENSSL_INCLUDE_DIR=$env:OPENSSL_INCLUDE_DIR"
    }
    
    cmake @cmakeArgs
```

**What This Does:**
1. **Detects OpenSSL installation path** - Checks four common installation locations
2. **Sets root directory** - Exports `OPENSSL_ROOT_DIR` for CMake
3. **Sets individual library paths** - Explicitly sets `OPENSSL_CRYPTO_LIBRARY` and `OPENSSL_SSL_LIBRARY`
4. **Sets include directory** - Exports `OPENSSL_INCLUDE_DIR` for headers
5. **Passes all paths to CMake** - Conditionally adds each path if found
6. **Validates installation** - Fails fast if OpenSSL not found

### 2. macOS Build Fix

**Changes to `.github/workflows/build.yml` and `.github/workflows/release.yml`:**

```yaml
- name: Setup CMake
  uses: jwlawson/actions-setup-cmake@v1.14
  with:
    cmake-version: '3.28.x'

- name: Install dependencies
  run: |
    brew install openssl@3
```

**What This Does:**
1. **Upgrades CMake** - Uses action to install CMake 3.28.x (newer than 3.25.x)
2. **Removes cmake from brew** - No longer installs CMake via brew (prevents version conflicts)
3. **Keeps OpenSSL** - Still installs OpenSSL 3.x via brew
4. **Ensures compatibility** - CMake 3.28 is compatible with nlohmann/json v3.11.2

---

## Technical Details

### OpenSSL Installation Paths (Windows)

Chocolatey may install OpenSSL to different locations depending on version:
- `C:\Program Files\OpenSSL` (standard)
- `C:\Program Files\OpenSSL-Win64` (64-bit specific)

Our fix checks both locations and uses whichever exists.

### CMake Version Requirements

| Component | Minimum CMake Version |
|-----------|----------------------|
| Our project (CMakeLists.txt) | 3.10 |
| nlohmann/json v3.11.2 | 3.5 |
| cpp-httplib v0.14.0 | 3.14 |
| GitHub Actions runner | 3.25+ (via action) |
| Our workflow | **3.28** (to ensure compatibility) |

### Environment Variable Setup

**Windows PowerShell:**
```powershell
echo "VARIABLE_NAME=value" >> $env:GITHUB_ENV
```

**Linux/macOS Bash:**
```bash
echo "VARIABLE_NAME=value" >> $GITHUB_ENV
```

---

## Files Modified

### 1. `.github/workflows/build.yml`

**Changes:**
- ✅ Added OpenSSL detection step for Windows
- ✅ Added `OPENSSL_ROOT_DIR` to CMake configure command (Windows)
- ✅ Upgraded CMake to 3.28.x for macOS
- ✅ Removed CMake from brew install (macOS)

### 2. `.github/workflows/release.yml`

**Changes:**
- ✅ Added OpenSSL detection step for Windows release builds
- ✅ Added `OPENSSL_ROOT_DIR` to CMake configure command (Windows)
- ✅ Upgraded CMake to 3.28.x for macOS release builds
- ✅ Removed CMake from brew install (macOS)

---

## Expected Outcomes

### Windows Build ✅

```
-- Found OpenSSL: C:/Program Files/OpenSSL-Win64/lib/libcrypto.lib (found version "3.6.0")
-- Found OpenSSL: C:/Program Files/OpenSSL-Win64/lib/libssl.lib
-- Configuring done
-- Generating done
-- Build files have been written to: D:/a/Budgeteer/Budgeteer/Backend/build
```

### macOS Build ✅

```
-- The C compiler identification is AppleClang 17.0.0.17000013
-- The CXX compiler identification is AppleClang 17.0.0.17000013
-- Found OpenSSL: /opt/homebrew/opt/openssl@3/lib/libcrypto.dylib (found version "3.5.2")
-- Configuring done
-- Generating done
-- Build files have been written to: /Users/runner/work/Budgeteer/Budgeteer/Backend/build
```

### Linux Build ✅

No changes needed - already working correctly.

---

## Testing Recommendations

### Local Testing

**Windows (PowerShell):**
```powershell
# Set OpenSSL path
$env:OPENSSL_ROOT_DIR = "C:\Program Files\OpenSSL-Win64"

# Build
cd Backend
cmake -B build -DCMAKE_BUILD_TYPE=Release -DOPENSSL_ROOT_DIR="$env:OPENSSL_ROOT_DIR"
cmake --build build --config Release
```

**macOS:**
```bash
# Ensure CMake 3.28+ is installed
cmake --version

# Build
cd Backend
export OPENSSL_ROOT_DIR=$(brew --prefix openssl@3)
cmake -B build -DCMAKE_BUILD_TYPE=Release -DOPENSSL_ROOT_DIR=$OPENSSL_ROOT_DIR
cmake --build build --config Release
```

### CI Testing

After pushing these changes:
1. Create a test branch
2. Push to trigger build workflow
3. Verify all three platforms (Windows, Linux, macOS) build successfully
4. Check workflow logs for confirmation messages

---

## Prevention for Future

### Best Practices

1. **Always specify OpenSSL path explicitly** on Windows builds
2. **Use GitHub Actions for CMake installation** instead of package managers
3. **Pin CMake versions** to ensure consistency across platforms
4. **Test locally** on all target platforms before pushing
5. **Use environment variables** for paths that may vary

### Monitoring

- Check [GitHub Actions](https://github.com/YK12321/Budgeteer/actions) after each push
- Review workflow logs for deprecation warnings
- Update dependency versions regularly

---

## Additional Notes

### Why Not Use vcpkg or Conan?

While package managers like vcpkg or Conan could simplify dependency management, we chose the current approach because:
- ✅ Minimal dependencies (only OpenSSL)
- ✅ FetchContent handles cpp-httplib and nlohmann/json automatically
- ✅ Faster CI builds (no package manager setup time)
- ✅ Simpler for competition judges to understand

### Alternative: OpenSSL via vcpkg

If issues persist, consider using vcpkg:

```yaml
- name: Setup vcpkg
  run: |
    git clone https://github.com/microsoft/vcpkg.git
    .\vcpkg\bootstrap-vcpkg.bat
    .\vcpkg\vcpkg install openssl:x64-windows

- name: Configure CMake
  run: |
    cd Backend
    cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake
```

---

## References

[1] GitHub Actions Documentation: https://docs.github.com/en/actions  
[2] CMake FindOpenSSL Module: https://cmake.org/cmake/help/latest/module/FindOpenSSL.html  
[3] nlohmann/json CMake Requirements: https://github.com/nlohmann/json#cmake  
[4] cpp-httplib Documentation: https://github.com/yhirose/cpp-httplib

---

**Status:** ✅ Fixes implemented and ready for testing  
**Next Step:** Push changes and verify CI/CD pipeline succeeds
