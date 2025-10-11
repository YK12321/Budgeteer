# 🚀 Budgeteer - Quick Start Guide

## Running the Complete Application

### Step 1: Start the Backend Server

Open a PowerShell terminal:

```powershell
cd Backend
.\start-http-server.ps1
```

You should see:
```
========================================
  Budgeteer Backend HTTP Server
========================================

✓ HTTP Server configured with endpoints
✓ CORS enabled for frontend access
✓ Ready to accept requests at http://localhost:8080
```

**Keep this terminal running!** The backend server needs to stay active.

### Step 2: Open the Frontend

Open a **new** PowerShell terminal:

```powershell
cd Frontend
.\open-frontend.ps1
```

Or simply open `Frontend/index.html` in your browser.

### Step 3: Use the Application

The frontend should now be connected to the backend! You can:

1. **Search for products** - Try searching for "milk", "chips", or "TV"
2. **Filter results** - Use store, category, and price filters
3. **View details** - Click on any item to see price history
4. **Create shopping lists** - Switch to Shopping List tab
5. **Use AI features** - Try the AI-powered shopping list generator

## Quick Test

To verify everything is working:

1. Open the browser console (F12)
2. Search for "milk" in the frontend
3. Check the backend terminal - you should see:
   ```
   [HTTP] GET /search?q=milk
   ```
4. Check the browser Network tab - you should see a successful request to `http://localhost:8080/search?q=milk`

## Troubleshooting

### Backend won't start
- **Error: Port in use** → Try a different port:
  ```powershell
  .\build\bin\Debug\BudgeteerAPI.exe --http --port 3000
  ```
  Then update `API_BASE_URL` in `Frontend/app-enhanced.js`

### Frontend shows no data
- **Check:** Is the backend running?
- **Test:** Open `http://localhost:8080/items` in your browser
- **Fix:** Make sure `USE_REAL_API = true` in `Frontend/app-enhanced.js`

### CORS errors
- **Check:** Backend has CORS enabled (it does by default)
- **Fix:** Use a local server for frontend instead of file:// protocol
  ```powershell
  cd Frontend
  python -m http.server 5500
  # Then open http://localhost:5500
  ```

## Project Structure

```
Budgeteer/
├── Backend/                    # C++ API Server
│   ├── src/                   # Source files
│   ├── include/               # Header files
│   ├── build/                 # Build output
│   ├── start-http-server.ps1  # Launch script
│   └── CMakeLists.txt         # Build configuration
│
├── Frontend/                  # Web Interface
│   ├── index.html            # Main page
│   ├── app-enhanced.js       # Application logic
│   ├── styles.css            # Styling
│   └── open-frontend.ps1     # Launch script
│
└── BACKEND_FRONTEND_INTEGRATION.md  # Detailed docs
```

## Key Features

### Backend (C++ HTTP Server)
- ✅ RESTful API with JSON responses
- ✅ Price comparison across stores
- ✅ Smart search with similarity matching
- ✅ LLM integration for AI features
- ✅ CORS enabled for frontend access

### Frontend (Modern JavaScript)
- ✅ Real-time product search
- ✅ Advanced filtering (store, category, price)
- ✅ Price history visualization
- ✅ Shopping list management
- ✅ AI-powered shopping list generator

## API Endpoints

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/items` | GET | Get all items |
| `/search?q=query` | GET | Search items |
| `/stores` | GET | Get all stores |
| `/categories` | GET | Get all categories |
| `/api/llm/query` | POST | Natural language query |
| `/api/llm/shopping-list` | POST | Generate AI shopping list |

See [BACKEND_FRONTEND_INTEGRATION.md](BACKEND_FRONTEND_INTEGRATION.md) for complete API documentation.

## Development Mode

### Backend Development
1. Make changes to C++ files
2. Rebuild: `cd build; cmake --build . --config Debug`
3. Restart: `.\start-http-server.ps1`

### Frontend Development
1. Make changes to HTML/JS/CSS files
2. Refresh browser (Ctrl+R or F5)
3. Hard refresh if needed (Ctrl+Shift+R)

### Testing Without Backend
Set `USE_REAL_API = false` in `Frontend/app-enhanced.js` to use mock data.

## Demo Scenario

Try this to see all features:

1. **Start both servers** (backend + frontend)
2. **Search** for "TV" - see price comparison across stores
3. **Click** on an item - view detailed price history
4. **Go to Shopping List** tab
5. **Try AI mode** - Enter: "snacks for a party under $20"
6. **See AI generate** optimal shopping list
7. **Compare prices** - Click "Compare Prices" button

## Next Steps

- ✅ Backend and frontend fully integrated
- ✅ HTTP API server working
- ✅ CORS configured
- ✅ Real-time data flow
- 🎯 Ready for presentation!

## Help

For detailed documentation, see:
- [BACKEND_FRONTEND_INTEGRATION.md](BACKEND_FRONTEND_INTEGRATION.md) - Complete integration guide
- [Backend/README.md](Backend/README.md) - Backend documentation
- [Frontend/README.md](Frontend/README.md) - Frontend documentation

---

**Built for YEC 2025 Programming Competition** 🏆
