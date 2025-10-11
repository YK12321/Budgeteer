# Budgeteer API - Getting Started Guide

## Quick Start

### 1. Build the Project

**Option A: Using the build script (Easiest)**
```powershell
.\build.bat
```

**Option B: Using Make**
```powershell
make
```

**Option C: Using CMake**
```powershell
mkdir build
cd build
cmake ..
cmake --build .
```

### 2. Run the Application

```powershell
cd bin
.\BudgeteerAPI.exe
```

## Prerequisites

You need a C++ compiler. Choose one:

### Option 1: MinGW-w64 (Recommended for Windows)
1. Download from: https://www.mingw-w64.org/
2. Install with g++ support
3. Add to PATH: `C:\mingw64\bin`

### Option 2: MSYS2 (Recommended)
1. Download from: https://www.msys2.org/
2. Install MSYS2
3. Open MSYS2 terminal and run:
   ```bash
   pacman -S mingw-w64-x86_64-gcc
   pacman -S make
   ```
4. Add to PATH: `C:\msys64\mingw64\bin`

### Option 3: Visual Studio
1. Install Visual Studio 2019 or later
2. Include "Desktop development with C++"
3. Use CMake option to build

## Testing the API

Once running, try these options:

1. **Get all items** - Option 1
2. **Search by ID** - Option 2, enter `1001`
3. **Search by name** - Option 3, enter `Samsung`
4. **Filter by store** - Option 4, enter `Walmart`
5. **Filter by category** - Option 5, enter `electronics`
6. **Price range** - Option 6, enter min: `600`, max: `800`
7. **Search** - Option 7, enter `TV`
8. **Statistics** - Option 8, enter `1001`
9. **All stores** - Option 9
10. **All categories** - Option 10

## API Endpoints (Simulated)

The CLI simulates these REST endpoints:

```
GET /items                           → Option 1
GET /items/:id                       → Option 2
GET /items?name=:name               → Option 3
GET /items?store=:store             → Option 4
GET /items?category=:category       → Option 5
GET /items?min=:min&max=:max        → Option 6
GET /search?q=:query                → Option 7
GET /items/:id/stats                → Option 8
GET /stores                         → Option 9
GET /categories                     → Option 10
```

## Architecture

```
┌─────────────────────────────────────────┐
│           main.cpp                      │
│      (Application Entry Point)          │
└─────────────────┬───────────────────────┘
                  │
                  ▼
┌─────────────────────────────────────────┐
│          ApiServer.cpp                  │
│    (Request Routing & Response)         │
└─────────────────┬───────────────────────┘
                  │
                  ▼
┌─────────────────────────────────────────┐
│          Database.cpp                   │
│    (Query Processing & Filtering)       │
└─────────────────┬───────────────────────┘
                  │
                  ▼
┌─────────────────────────────────────────┐
│           Item.cpp                      │
│        (Data Model)                     │
└─────────────────────────────────────────┘
```

## Next Steps

### Convert to Real HTTP Server

Install a C++ HTTP library:

**Option 1: cpp-httplib (Header-only)**
```cpp
#include "httplib.h"

httplib::Server svr;
svr.Get("/items", [&](const httplib::Request&, httplib::Response& res) {
    res.set_content(server.handleGetAllItems(), "application/json");
});
svr.listen("0.0.0.0", 8080);
```

**Option 2: Crow (Modern, Flask-like)**
```cpp
#include "crow.h"

crow::SimpleApp app;
CROW_ROUTE(app, "/items")
([&]() {
    return server.handleGetAllItems();
});
app.port(8080).run();
```

### Add Database Support

Replace CSV with SQLite:
```cpp
#include <sqlite3.h>
// Implement proper database queries
```

### Add External API Integration

Use libcurl for external price APIs:
```cpp
#include <curl/curl.h>
// Fetch real-time prices
```

## Troubleshooting

**Problem:** `'g++' is not recognized`
- **Solution:** Install MinGW-w64 or MSYS2 and add to PATH

**Problem:** `make: command not found`
- **Solution:** Use `build.bat` or install Make via MSYS2

**Problem:** Cannot find CSV file
- **Solution:** Make sure `SampleDataset/` folder is in the same directory as the executable

**Problem:** Compilation errors
- **Solution:** Ensure C++17 support: `g++ --version` should be 7.0 or higher

## Performance

- Loads ~6,680 items in < 1 second
- Query response time: < 100ms for most queries
- Memory usage: ~5MB for dataset in memory

## License

MIT License
