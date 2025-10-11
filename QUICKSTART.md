# Quick Start Guide 🚀

## Build the Backend (5 minutes)

```powershell
# Navigate to backend directory
cd Backend

# Create build directory
mkdir build
cd build

# Generate build files
cmake ..

# Build the project
cmake --build . --config Debug

# Navigate to executable
cd bin\Debug

# Run the application
.\BudgeteerAPI.exe
```

## Test Backend Features

Once running, try these options:

### Test Database Search (Option 7)
```
Enter option: 7
Enter search term: milk
```

### Test Natural Language Query (Option 13)
```
Enter option: 13
Enter natural language query: Find me snacks under $10
```

### Test AI Shopping List (Option 14)
```
Enter option: 14
Describe what you need: I need groceries for a family dinner
```

### Test Budget Insight (Option 15)
```
Enter option: 15
```

## Run the Frontend (2 minutes)

### Method 1: Direct Browser Access
1. Navigate to `Frontend` folder
2. Double-click `index.html`
3. Opens in your default browser

### Method 2: Local Server
```powershell
# Navigate to frontend directory
cd Frontend

# Start server (Python 3)
python -m http.server 8000

# Open browser to:
# http://localhost:8000
```

## Test Frontend Features

### 1. Test Search
- Click on the search bar
- Type "samsung tv"
- Click "Search" button
- Browse results
- Click on any item for details

### 2. Test Manual Shopping List
- Click "Shopping List" tab
- Ensure "Manual List" is selected
- Type "Milk" and press Enter
- Type "Bread" and press Enter
- Type "Eggs" and press Enter
- Click "Compare Prices"

### 3. Test AI Shopping List
- Click "Shopping List" tab
- Click "AI-Powered" button
- Type: "I need snacks for a party"
- Click "Generate Smart List"
- Click "Save to My List"
- Switch back to "Manual List" to see items

### 4. Test Price Comparison
- Add several items to manual list
- Click "Compare Prices"
- Review the comparison table
- Check which store offers best total

## Troubleshooting

### Backend Won't Build
**Issue:** CMake errors
**Solution:**
```powershell
# Try using Visual Studio directly
cd Backend
# Open BudgeteerAPI.sln in Visual Studio
# Build from Visual Studio
```

### Backend Can't Find CSV File
**Issue:** "Failed to load database"
**Solution:**
```powershell
# Make sure you're in the correct directory
cd Backend\build\bin\Debug

# Check if SampleDataset folder exists
dir SampleDataset

# If not, copy it manually
xcopy /E /I ..\..\..\..\Backend\SampleDataset .\SampleDataset
```

### Frontend Not Updating
**Issue:** Changes not visible
**Solution:**
1. Hard refresh browser: `Ctrl+Shift+R`
2. Clear browser cache
3. Close and reopen browser

### Shopping List Not Saving
**Issue:** List disappears on refresh
**Solution:**
- Check browser console for errors (F12)
- Ensure localStorage is enabled
- Try a different browser

## Expected Results

### Backend Output Example
```
========================================
    Budgeteer Backend API Server
    with LLM & Real-time APIs
========================================

Initializing API Server on port 8080...
Real-time APIs: DISABLED (using sample dataset)
API Server initialized successfully!
Loaded 6678 items from dataset.

========================================
Server ready to handle requests!
Port: 8080
Mode: CLI Interactive
Real-time APIs: DISABLED
========================================

========================================
      Budgeteer API Menu
========================================
DATABASE QUERIES:
1.  Get all items
...
```

### Frontend Display
- Search results grid with product cards
- Shopping List tab with manual/AI modes
- Price comparison table
- Store badges with colors:
  - Walmart: Blue (#0071CE)
  - Loblaws: Red (#ED1B24)
  - Costco: Dark Blue (#0063A5)

## Next Steps

1. ✅ **Explore Features**: Try all menu options in backend
2. ✅ **Test Shopping List**: Create lists and compare prices
3. ✅ **Review Code**: Check implementation files
4. 🔄 **Enable Real APIs**: Follow IMPLEMENTATION.md for API setup
5. 🔄 **Deploy**: Host on a server for remote access

## Quick Reference

### Backend Commands
```powershell
# Standard mode
.\BudgeteerAPI.exe

# With real-time APIs
.\BudgeteerAPI.exe --realtime

# HTTP server mode
.\BudgeteerAPI.exe --http --port 8080

# Show help
.\BudgeteerAPI.exe --help
```

### Frontend URLs
- **Local file**: `file:///path/to/Frontend/index.html`
- **Python server**: `http://localhost:8000`
- **Live Server**: Check VS Code output for URL

### Key Files to Review
- `Backend/src/LLMInterface.cpp` - Natural language processing
- `Backend/src/StoreApiClient.cpp` - API integration framework
- `Frontend/app-enhanced.js` - Shopping list logic
- `IMPLEMENTATION.md` - Complete documentation

## Tips

💡 **Backend**: Start with database queries before testing LLM features

💡 **Frontend**: Use browser DevTools (F12) to see console logs

💡 **Testing**: Use the manual shopping list first to understand the flow

💡 **Development**: Keep both backend and frontend terminals open

## Success Checklist

- [ ] Backend builds without errors
- [ ] Backend loads CSV dataset (6678 items)
- [ ] Frontend opens in browser
- [ ] Search functionality works
- [ ] Can add items to shopping list
- [ ] Price comparison displays results
- [ ] AI shopping list generates items
- [ ] Can switch between manual/AI modes
- [ ] Shopping list persists after refresh

If all checkboxes are ✅, you're ready to go! 🎉

## Need Help?

1. Check `IMPLEMENTATION.md` for detailed documentation
2. Review code comments in source files
3. Check browser console for frontend errors
4. Check terminal output for backend errors
5. Ensure all files are in correct locations

Happy shopping! 🛒
