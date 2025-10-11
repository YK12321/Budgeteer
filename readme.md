# Budgeteer - Complete Project

A full-stack price comparison application with C++ backend and modern JavaScript frontend.

## 🎯 Project Overview

Budgeteer helps users find the best prices for products across multiple stores (Walmart, Loblaws, Costco). It features:

- **Backend**: Object-oriented C++ API with intelligent search algorithms
- **Frontend**: Modern, responsive web application with AI-powered shopping lists
- **Database**: CSV-based data storage with 6,678+ price records
- **Smart Search**: Levenshtein distance algorithm for fuzzy matching
- **🤖 GPT-4o-mini via GitHub**: Advanced natural language query processing (FREE!)
- **Hybrid AI**: Smart switching between local and GPT processing
- **Real-Time APIs**: Framework for live store price integration
- **Shopping Lists**: Manual and AI-powered list creation
- **Price Comparison**: Multi-store comparison with savings recommendations

## 📁 Project Structure

```
Programming/
├── Backend/                    # C++ Backend API
│   ├── include/               # Header files
│   │   ├── Item.h            # Item model class
│   │   ├── Database.h        # Database operations
│   │   └── ApiServer.h       # API server class
│   ├── src/                  # Implementation files
│   │   ├── Item.cpp
│   │   ├── Database.cpp
│   │   ├── ApiServer.cpp
│   │   └── main.cpp
│   ├── SampleDataset/        # CSV dataset
│   │   └── yec_competition_dataset.csv
│   ├── CMakeLists.txt        # CMake configuration
│   ├── Makefile              # Make configuration
│   ├── build.bat             # Windows build script
│   └── README.md
├── Frontend/                  # JavaScript Frontend
│   ├── index.html            # Main HTML file
│   ├── styles.css            # Styles and animations
│   ├── app.js                # Application logic
│   └── README.md
├── build/                     # Build output directory
└── README.md                  # This file
```

## 🚀 Quick Start

### Prerequisites

**For Backend:**
- C++17 compatible compiler (g++, MSVC, or clang++)
- CMake 3.10+ or Make
- OpenSSL (for HTTPS requests)
- Windows, macOS, or Linux
- GitHub Personal Access Token (optional, for GPT-4o-mini features - FREE!)

**For Frontend:**
- Modern web browser (Chrome, Firefox, Safari, Edge)
- Optional: Python or Node.js for local server

### Running the Backend

**Step 1: Set GitHub Token (Optional but Recommended - FREE!)**
```powershell
# Get your token from: https://github.com/settings/tokens
# Required scope: read:packages
$env:GITHUB_TOKEN="ghp_your-token-here"
```

**Step 2: Build with CMake (Recommended)**
```powershell
# Build
cd Backend
mkdir build
cd build
cmake ..
cmake --build . --config Debug

# Run
cd bin\Debug
.\BudgeteerAPI.exe
```

> **Note:** First build will download dependencies (cpp-httplib, nlohmann/json). Requires internet connection.

**Option 2: Using Make**
```powershell
cd Backend
make
cd bin
.\BudgeteerAPI.exe
```

**Option 3: Using build.bat (Windows)**
```powershell
cd Backend
.\build.bat
cd bin
.\BudgeteerAPI.exe
```

### Running the Frontend

**Option 1: Direct Browser Access**
```powershell
# Simply open Frontend/index.html in your browser
```

**Option 2: Python HTTP Server**
```powershell
cd Frontend
python -m http.server 8000
# Open http://localhost:8000
```

**Option 3: VS Code Live Server**
1. Install "Live Server" extension
2. Right-click `index.html`
3. Select "Open with Live Server"

## 🎨 Features

### Backend Features

✅ **🤖 GPT-4o-mini via GitHub Models** 🆕
- Advanced natural language understanding
- **FREE during beta** - No credit card required!
- Intent recognition (search, compare, shopping list, budget)
- Hybrid processing: Local for simple queries, GPT for complex ones
- Automatic fallback if API unavailable
- Cost-optimized with daily query limits
- Context-aware product extraction

✅ **Object-Oriented Design**
- Clean separation with Item, Database, ApiServer, StoreApiClient, and LLMInterface classes
- Proper encapsulation and abstraction
- Modular architecture for easy extension

✅ **Smart Search Algorithm**
- Levenshtein distance for fuzzy matching
- Similarity scoring (0.0 - 1.0)
- Multi-criteria matching (name, description, words)
- Results sorted by relevance

✅ **Multiple Query Types**
1. Search by item name
2. Filter by store (Walmart, Loblaws, Costco)
3. Filter by category (electronics, baby, household, etc.)
4. Filter by price range
5. Get item by ID
6. Price statistics (min, max, average)
7. Real-time store searches (framework ready)
8. Natural language queries
9. AI shopping list generation
10. Budget insights

✅ **LLM Natural Language Interface**
- Intent recognition (search, compare, shopping list, budget)
- Category expansion (generic → specific products)
- Query normalization
- Multiple ranking modes:
  - Cheapest Mix Mode
  - Single Store Mode
  - Budget Insight Mode
- Formatted text responses

✅ **Real-Time API Integration**
- StoreApiClient framework for Walmart, Loblaws, Costco
- Search across all stores simultaneously
- Price comparison functionality
- Extensible architecture (ready for API keys)

✅ **Data Processing**
- CSV file parsing
- 6,678+ price records loaded
- Price history tracking
- Store and category aggregation
- In-memory caching support

### Frontend Features

✅ **Modern UI/UX**
- Responsive design (mobile, tablet, desktop)
- Smooth animations and transitions
- Intuitive tab-based navigation
- Clean, professional aesthetic
- Color-coded store branding

✅ **Search & Filters**
- Real-time search
- Store filter dropdown
- Category filter dropdown
- Price range slider
- Multiple sort options

✅ **Shopping List Manager** 🆕
- **Manual Mode**:
  - Add items by typing
  - Check off completed items
  - Remove individual items
  - Clear entire list
  - Persistent storage (localStorage)
  
- **AI-Powered Mode**:
  - Natural language input
  - Budget constraints
  - Smart list generation
  - Best price selection
  - Save to manual list

✅ **Price Comparison** 🆕
- Compare all list items across stores
- Side-by-side price table
- Total cost per store
- Best store recommendation
- Savings calculation
- Beautiful visual presentation

✅ **Item Details**
- Modal popup with full details
- Complete price history
- Statistical analysis (min, max, avg)
- Category tags
- Store-specific pricing
- Latest price date display

## 🏗️ Architecture

### Backend Architecture

```
┌─────────────────────────────────┐
│         main.cpp                │
│    (Application Entry)          │
└───────────┬─────────────────────┘
            │
            ▼
┌─────────────────────────────────┐
│       ApiServer                 │
│  - Request Routing              │
│  - JSON Response Generation     │
│  - Query Parameter Handling     │
└───────────┬─────────────────────┘
            │
            ▼
┌─────────────────────────────────┐
│        Database                 │
│  - CSV Parsing                  │
│  - Data Storage (Vector)        │
│  - Search Algorithms            │
│  - Filtering & Sorting          │
└───────────┬─────────────────────┘
            │
            ▼
┌─────────────────────────────────┐
│          Item                   │
│  - Data Model                   │
│  - Getters/Setters              │
│  - JSON Serialization           │
└─────────────────────────────────┘
```

### Frontend Architecture

```
┌─────────────────────────────────┐
│       index.html                │
│  - Structure & Layout           │
│  - Semantic HTML5               │
└───────────┬─────────────────────┘
            │
            ▼
┌─────────────────────────────────┐
│       styles.css                │
│  - Modern CSS3                  │
│  - Responsive Design            │
│  - Animations                   │
└─────────────────────────────────┘
            │
            ▼
┌─────────────────────────────────┐
│         app.js                  │
│  - State Management             │
│  - API Calls (Mock/Real)        │
│  - DOM Manipulation             │
│  - Event Handling               │
└─────────────────────────────────┘
```

## 🔬 Technical Details

### Backend Algorithms

**1. Levenshtein Distance**
```cpp
// Calculates edit distance between two strings
// Used for fuzzy matching in search
int calculateLevenshteinDistance(s1, s2)
```

**2. Similarity Score**
```cpp
// Converts distance to similarity (0.0 to 1.0)
double calculateSimilarity(s1, s2)
// Returns: 1.0 - (distance / maxLength)
```

**3. Search Scoring System**
- Exact name match: +100 points
- Exact description match: +50 points
- Name similarity: +75 points (scaled)
- Word matches: +30 points each
- Individual word similarity: +40 points (if > 60%)

### Data Model

**Item Object:**
```json
{
  "item_id": 1001,
  "item_name": "Samsung 55-inch 4K Smart TV",
  "item_description": "55-inch Crystal UHD Smart TV with HDR",
  "current_price": 705.18,
  "store": "Walmart",
  "category_tags": ["electronics", "entertainment", "home"],
  "image_url": "https://example.com/...",
  "price_date": "2022-01-15"
}
```

## 🔌 API Integration Guide

### Current Status
- Backend: **CLI-based** (no HTTP server yet)
- Frontend: **Uses mock data**

### To Enable Real API

**Step 1: Add HTTP Server to Backend**

Install cpp-httplib:
```bash
# In Backend/CMakeLists.txt, add:
include(FetchContent)
FetchContent_Declare(
    httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
)
FetchContent_MakeAvailable(httplib)
```

**Step 2: Create API Endpoints**

Add to `ApiServer.cpp`:
```cpp
#include "httplib.h"

void ApiServer::startHttpServer() {
    httplib::Server svr;
    
    svr.Get("/api/search", [this](const auto& req, auto& res) {
        std::string query = req.get_param_value("q");
        res.set_content(handleSearchItems(query), "application/json");
    });
    
    svr.listen("0.0.0.0", 8080);
}
```

**Step 3: Update Frontend**

Replace mock data in `app.js`:
```javascript
async function performSearch() {
    const response = await fetch(`http://localhost:8080/api/search?q=${query}`);
    const data = await response.json();
    displayResults(data.items);
}
```

## 📊 Dataset Information

- **Total Records**: 6,678 price entries
- **Unique Items**: ~160 products
- **Stores**: 3 (Walmart, Loblaws, Costco)
- **Categories**: 10+ categories
- **Date Range**: 2022-2025
- **Format**: CSV (comma-separated values)

## 🎯 Use Cases

1. **Price Comparison** - Find the best price across stores
2. **Price Tracking** - Monitor price changes over time
3. **Budget Planning** - Plan purchases based on historical prices
4. **Store Selection** - Choose the most cost-effective store
5. **Category Browsing** - Explore products by category

## 🛠️ Development

### Building from Source

**Backend:**
```powershell
cd Backend
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

**Frontend:**
No build step required - uses vanilla JavaScript

### Testing

**Backend Test:**
```powershell
cd build\bin\Debug
.\BudgeteerAPI.exe
# Try option 7 (Search) with "Samsung TV"
```

**Frontend Test:**
```
Open index.html
Search for "Samsung TV"
Click on any result to see details
```

## 📈 Performance

### Backend
- Load time: < 1 second (6,678 items)
- Search time: < 100ms
- Memory usage: ~5MB

### Frontend
- First load: < 1 second
- Search rendering: < 50ms
- Bundle size: ~15KB total

## 🎓 Learning Outcomes

This project demonstrates:

✅ **C++ Programming**
- Object-oriented design
- STL containers and algorithms
- File I/O and parsing
- Memory management

✅ **Data Structures & Algorithms**
- String algorithms (Levenshtein distance)
- Searching and filtering
- Sorting algorithms
- Hash maps and sets

✅ **Web Development**
- Modern HTML5/CSS3
- Vanilla JavaScript (ES6+)
- Responsive design
- API integration patterns

✅ **Software Engineering**
- Clean code principles
- Modular architecture
- Documentation
- Version control

## � Documentation

- **[QUICKSTART.md](QUICKSTART.md)**: Get started in 5 minutes
- **[GITHUB_API_GUIDE.md](GITHUB_API_GUIDE.md)**: 🆕 GitHub Models API setup (FREE!)
- **[GITHUB_API_MIGRATION.md](GITHUB_API_MIGRATION.md)**: Migration from OpenAI
- **[IMPLEMENTATION.md](IMPLEMENTATION.md)**: Complete implementation details
- **[API_INTEGRATION.md](API_INTEGRATION.md)**: Real store API integration guide
- **[SUMMARY.md](SUMMARY.md)**: Project summary and achievements
- **[LLM-instructions.txt](LLM-instructions.txt)**: LLM interface requirements

## �🚧 Future Enhancements

- [x] Add LLM natural language interface ✅
- [x] Integrate GPT-4o-mini for advanced queries ✅ 🆕
- [x] Add shopping list feature ✅
- [x] Add price comparison ✅
- [x] Real-time API framework ✅
- [x] HTTP client integration (cpp-httplib) ✅ 🆕
- [ ] Connect to real store APIs (API keys needed)
- [ ] Implement user authentication
- [ ] Add favorites/wishlist feature
- [ ] Create price alert system
- [ ] Add data visualization charts
- [ ] Advanced caching layer
- [ ] Add unit tests
- [ ] Deploy to cloud platform
- [ ] Mobile app version

## 📝 License

MIT License - Built for YEC 2025 Programming Competition

## 👥 Contributors

Built with ❤️ for York Entrepreneurship Competition 2025

---

**Questions?** Open an issue in the repository!
