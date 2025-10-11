# Backend-Frontend Integration Guide

## Overview
This document explains how the Budgeteer backend (C++ API) and frontend (HTML/JavaScript) are connected and how to run them together.

## Architecture

```
┌─────────────────┐         HTTP/JSON          ┌──────────────────┐
│                 │    ←──────────────────→     │                  │
│  Frontend       │                             │  Backend API     │
│  (HTML/JS)      │    localhost:8080           │  (C++ Server)    │
│                 │                             │                  │
└─────────────────┘                             └──────────────────┘
      ↓                                                   ↓
  index.html                                        Database.cpp
  app-enhanced.js                                   ApiServer.cpp
  styles.css                                        LLMInterface.cpp
```

## Backend API Server

### Technology Stack
- **C++17** - Core language
- **cpp-httplib** - HTTP server library
- **nlohmann/json** - JSON parsing
- **OpenSSL** - HTTPS support (optional)

### Starting the Backend

#### Option 1: PowerShell Script (Recommended)
```powershell
cd Backend
.\start-http-server.ps1
```

#### Option 2: Manual Command
```powershell
cd Backend
.\build\bin\Debug\BudgeteerAPI.exe --http
```

#### Option 3: Custom Port
```powershell
.\build\bin\Debug\BudgeteerAPI.exe --http --port 3000
```

### Available Endpoints

#### Basic Endpoints
- `GET /` - Server status
- `GET /items` - Get all items
- `GET /items/:id` - Get item by ID
- `GET /items/:id/stats` - Get price statistics for item
- `GET /stores` - Get all stores
- `GET /categories` - Get all categories

#### Search Endpoints
- `GET /search?q=query` - Search items by keyword
- `GET /search?name=name` - Search by name
- `GET /search?store=store` - Filter by store
- `GET /search?category=category` - Filter by category
- `GET /search?min=X&max=Y` - Filter by price range

#### LLM/AI Endpoints
- `POST /api/llm/query` - Natural language query
  ```json
  {"query": "find cheap snacks"}
  ```
- `POST /api/llm/shopping-list` - Generate AI shopping list
  ```json
  {"prompt": "party snacks under $50"}
  ```
- `POST /api/llm/budget-insight` - Get budget insights
  ```json
  {"items": [...]}
  ```

#### Real-time Endpoints (Database fallback)
- `GET /api/realtime/search?q=query` - Store search
- `GET /api/realtime/compare?product=name` - Price comparison

### CORS Configuration
The backend is configured to allow cross-origin requests from the frontend:
- `Access-Control-Allow-Origin: *`
- `Access-Control-Allow-Methods: GET, POST, OPTIONS`
- `Access-Control-Allow-Headers: Content-Type`

## Frontend Application

### Technology Stack
- **HTML5** - Structure
- **Modern JavaScript (ES6+)** - Logic
- **CSS3** - Styling
- **Fetch API** - HTTP requests

### Opening the Frontend

#### Option 1: File Protocol (Simple)
```powershell
cd Frontend
start index.html
```

#### Option 2: Local Server (Better)
Using Python:
```powershell
cd Frontend
python -m http.server 5500
# Open http://localhost:5500
```

Using Node.js (http-server):
```powershell
cd Frontend
npx http-server -p 5500
# Open http://localhost:5500
```

Using VS Code Live Server extension:
1. Install "Live Server" extension
2. Right-click `index.html`
3. Select "Open with Live Server"

### Frontend Configuration

The frontend can work in two modes:

#### Real API Mode (Default)
```javascript
const API_BASE_URL = 'http://localhost:8080';
const USE_REAL_API = true;
```

#### Mock Data Mode (Testing without backend)
```javascript
const API_BASE_URL = 'http://localhost:8080';
const USE_REAL_API = false;
```

Edit `Frontend/app-enhanced.js` to switch modes.

## Complete Setup Instructions

### 1. Build the Backend
```powershell
cd Backend
mkdir build -Force
cd build
cmake ..
cmake --build . --config Debug
cd ..
```

### 2. Start the Backend Server
```powershell
# From Backend directory
.\start-http-server.ps1
```

You should see:
```
========================================
  Starting HTTP Server on port 8080
========================================
✓ HTTP Server configured with endpoints
✓ CORS enabled for frontend access
✓ Ready to accept requests at http://localhost:8080
```

### 3. Open the Frontend
```powershell
# In a new terminal
cd Frontend
start index.html
```

Or use a local server for better experience.

### 4. Test the Connection

Open browser console (F12) and check:
- Network tab should show requests to `http://localhost:8080`
- Console should not show CORS errors
- Items should load from backend (not mock data)

## API Integration Examples

### JavaScript Fetch Examples

#### Get All Items
```javascript
fetch('http://localhost:8080/items')
    .then(response => response.json())
    .then(data => {
        console.log(`Found ${data.count} items`);
        console.log(data.items);
    });
```

#### Search Items
```javascript
fetch('http://localhost:8080/search?q=milk')
    .then(response => response.json())
    .then(data => {
        console.log(`Found ${data.count} matches`);
    });
```

#### Generate AI Shopping List
```javascript
fetch('http://localhost:8080/api/llm/shopping-list', {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify({
        prompt: 'snacks for a party under $50'
    })
})
    .then(response => response.json())
    .then(data => {
        console.log('Shopping List:', data.shopping_list);
    });
```

## Troubleshooting

### Backend Issues

**Problem: "Failed to start HTTP server"**
- Solution: Port 8080 may be in use. Try a different port:
  ```powershell
  .\build\bin\Debug\BudgeteerAPI.exe --http --port 3000
  ```
  Update frontend `API_BASE_URL` to match.

**Problem: "cpp-httplib not found"**
- Solution: Rebuild the project:
  ```powershell
  cd build
  cmake ..
  cmake --build . --config Debug
  ```

**Problem: "Failed to load database"**
- Solution: Make sure `SampleDataset/yec_competition_dataset.csv` exists
- Check the executable is running from the correct directory

### Frontend Issues

**Problem: CORS errors**
- Solution: Backend must be running with CORS enabled (default)
- Don't open `index.html` directly; use a local server

**Problem: "Failed to fetch"**
- Solution: Backend server must be running
- Check backend is on `http://localhost:8080`
- Verify network tab in browser dev tools

**Problem: No data loading**
- Solution: Open browser console (F12) and check for errors
- Verify `USE_REAL_API = true` in `app-enhanced.js`
- Test backend directly: `http://localhost:8080/items`

### Connection Test

Test if backend is running:
```powershell
# Using curl (if installed)
curl http://localhost:8080/

# Using Invoke-WebRequest (PowerShell)
Invoke-WebRequest -Uri http://localhost:8080/ | Select-Object Content
```

Should return:
```json
{"message":"Budgeteer API Server","version":"1.0","status":"running"}
```

## Development Workflow

### Making Changes

**Backend Changes:**
1. Edit C++ source files
2. Rebuild: `cd build; cmake --build . --config Debug`
3. Restart server: `.\start-http-server.ps1`

**Frontend Changes:**
1. Edit HTML/JS/CSS files
2. Refresh browser (no rebuild needed)
3. Hard refresh (Ctrl+F5) if styles not updating

### Testing

**Test Backend Only:**
```powershell
# Get all items
Invoke-RestMethod -Uri http://localhost:8080/items

# Search
Invoke-RestMethod -Uri "http://localhost:8080/search?q=milk"

# Get stores
Invoke-RestMethod -Uri http://localhost:8080/stores
```

**Test Frontend Only:**
Set `USE_REAL_API = false` in `app-enhanced.js` to use mock data.

## Performance Notes

- Backend handles ~100 items/request efficiently
- First load may be slow due to CSV parsing
- Subsequent requests are fast (in-memory database)
- LLM queries may take 1-2 seconds (GPT API calls)

## Security Notes

⚠️ **Development Mode Only**
- CORS is wide open (`*`)
- No authentication
- No rate limiting
- Not suitable for production

For production deployment:
1. Restrict CORS to specific origins
2. Add authentication (API keys, OAuth)
3. Implement rate limiting
4. Use HTTPS only
5. Validate all inputs

## Next Steps

### Enhancements
- [ ] Add WebSocket support for real-time updates
- [ ] Implement caching layer (Redis)
- [ ] Add request logging and analytics
- [ ] Create Docker containers for easy deployment
- [ ] Add automated tests for API endpoints

### Features
- [ ] User authentication
- [ ] Save shopping lists to database
- [ ] Price alert notifications
- [ ] Store location integration
- [ ] Mobile responsive design

## Resources

- [cpp-httplib Documentation](https://github.com/yhirose/cpp-httplib)
- [nlohmann/json Documentation](https://json.nlohmann.me/)
- [Fetch API Reference](https://developer.mozilla.org/en-US/docs/Web/API/Fetch_API)
- [CORS Guide](https://developer.mozilla.org/en-US/docs/Web/HTTP/CORS)

## Support

If you encounter issues:
1. Check this guide's troubleshooting section
2. Review console logs (backend terminal and browser)
3. Test endpoints individually with curl/PowerShell
4. Verify all files are in correct locations

---

**Last Updated:** January 2025
**Version:** 1.0
