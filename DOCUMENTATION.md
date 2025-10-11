# Budgeteer - Complete Documentation

**Version:** 1.0  
**Last Updated:** October 11, 2025  
**Competition:** York Entrepreneurship Competition (YEC) 2025

---

## Table of Contents

1. [Project Overview](#project-overview)
2. [System Architecture](#system-architecture)
3. [Installation & Setup](#installation--setup)
4. [Backend Documentation](#backend-documentation)
5. [Frontend Documentation](#frontend-documentation)
6. [API Reference](#api-reference)
7. [Features](#features)
8. [Usage Guide](#usage-guide)
9. [Development Guide](#development-guide)
10. [Troubleshooting](#troubleshooting)
11. [Performance & Optimization](#performance--optimization)
12. [Security](#security)

---

## Project Overview

### What is Budgeteer?

Budgeteer is an intelligent price comparison platform that helps users find the best deals across multiple stores (Walmart, Loblaws, and Costco). It combines traditional search capabilities with AI-powered natural language processing to provide a superior shopping experience.

### Key Features

- **Smart Search**: Intelligent search with similarity matching
- **AI-Powered Assistant**: "Budgie" chatbot powered by GPT-4o-mini
- **Multi-Store Comparison**: Compare prices across multiple retailers
- **Price History**: Track price changes over time
- **Budget Insights**: AI-generated shopping recommendations
- **Shopping List Generation**: Natural language shopping list creation
- **Responsive Design**: Works on desktop, tablet, and mobile

### Technology Stack

#### Backend
- **Language**: C++17
- **Build System**: CMake
- **HTTP Server**: cpp-httplib
- **JSON Parsing**: nlohmann/json
- **SSL/TLS**: OpenSSL
- **AI Integration**: GitHub Models API (GPT-4o-mini)

#### Frontend
- **HTML5**: Semantic markup
- **CSS3**: Modern styling with Grid & Flexbox
- **JavaScript**: Vanilla JS (no framework dependencies)

---

## System Architecture

### High-Level Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                        Frontend                              │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐     │
│  │ Basic Search │  │  AI Search   │  │ Price History│     │
│  │    Mode      │  │     Mode     │  │    Modal     │     │
│  └──────────────┘  └──────────────┘  └──────────────┘     │
└────────────────────────┬────────────────────────────────────┘
                         │ HTTP/JSON
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                    Backend API Server                        │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐     │
│  │  ApiServer   │  │LLMInterface  │  │StoreApiClient│     │
│  │   (HTTP)     │  │  (AI Logic)  │  │  (Search)    │     │
│  └──────┬───────┘  └──────┬───────┘  └──────┬───────┘     │
│         │                  │                  │              │
│         └──────────────────┼──────────────────┘              │
│                            │                                 │
│                    ┌───────▼────────┐                        │
│                    │    Database    │                        │
│                    │  (CSV Parser)  │                        │
│                    └───────┬────────┘                        │
└────────────────────────────┼────────────────────────────────┘
                             │
                    ┌────────▼─────────┐
                    │   Sample Dataset │
                    │      (CSV)       │
                    └──────────────────┘
                             
                    ┌──────────────────┐
                    │ GitHub Models API│
                    │   (GPT-4o-mini)  │
                    └──────────────────┘
```

### Component Breakdown

#### 1. **Frontend Layer**
- **Search Interface**: Dual-mode search (basic and AI-powered)
- **Result Display**: Grid layout with filtering and sorting
- **Modal System**: Detailed item views with price history
- **Responsive Layout**: Mobile-first design approach

#### 2. **Backend API Layer**
- **ApiServer**: HTTP request handling and routing
- **LLMInterface**: Natural language processing and AI integration
- **StoreApiClient**: Product search orchestration
- **Database**: CSV data loading and querying

#### 3. **Data Layer**
- **CSV Dataset**: Product information with historical prices
- **GitHub Models API**: AI-powered query processing

### Data Flow

#### Basic Search Flow:
```
User Input → Frontend → Backend API → Database → Search Algorithm 
→ Results → JSON Response → Frontend Display
```

#### AI Search Flow:
```
User Query → Frontend → Backend API → LLMInterface → GPT-4o-mini 
→ Intent Analysis → Product Search → Cherry-Picking → Reasoning 
→ Validation → Results → JSON Response → Frontend Display
```

---

## Installation & Setup

### Prerequisites

#### Backend Requirements:
- **C++17 Compatible Compiler**:
  - Windows: Visual Studio 2019+ or MinGW-w64
  - Linux: GCC 7.0+ or Clang 5.0+
  - macOS: Xcode 10+ or Clang 5.0+
- **CMake 3.10+**
- **OpenSSL** (for HTTPS support)
- **Git** (for cloning dependencies)

#### Frontend Requirements:
- Modern web browser (Chrome, Firefox, Safari, or Edge)
- Optional: Local web server for development

### Quick Start (30 seconds)

```powershell
# 1. Clone repository
git clone https://github.com/YK12321/Budgeteer.git
cd Budgeteer

# 2. Get GitHub Personal Access Token
# Visit: https://github.com/settings/tokens
# Create token with 'read:packages' scope

# 3. Set environment variable
$env:GITHUB_TOKEN="ghp_your-token-here"

# 4. Build backend
cd Backend
cmake -B build
cmake --build build --config Debug

# 5. Run backend
.\build\bin\Debug\BudgeteerAPI.exe --http

# 6. Open frontend
cd ..\Frontend
Start-Process "index.html"
```

### Detailed Setup

#### Step 1: Backend Setup

##### Windows with Visual Studio:

```powershell
# Navigate to Backend directory
cd Backend

# Generate Visual Studio solution
cmake -B build -G "Visual Studio 16 2019"

# Build the project
cmake --build build --config Debug

# Run the server
.\build\bin\Debug\BudgeteerAPI.exe --http
```

##### Windows with MinGW:

```powershell
# Install MinGW-w64 if not already installed
# Download from: https://mingw-w64.org/

# Generate Makefiles
cmake -B build -G "MinGW Makefiles"

# Build
cmake --build build

# Run
.\build\bin\BudgeteerAPI.exe --http
```

##### Linux/macOS:

```bash
# Navigate to Backend directory
cd Backend

# Generate build files
cmake -B build

# Build
cmake --build build

# Run
./build/bin/BudgeteerAPI --http
```

#### Step 2: GitHub Token Setup

1. **Create GitHub Personal Access Token**:
   - Visit: https://github.com/settings/tokens
   - Click "Generate new token (classic)"
   - Give it a descriptive name (e.g., "Budgeteer API")
   - Select scope: `read:packages`
   - Click "Generate token"
   - **Copy the token immediately** (you won't see it again)

2. **Set Environment Variable**:

   **Windows PowerShell**:
   ```powershell
   $env:GITHUB_TOKEN="ghp_your-token-here"
   ```

   **Windows Command Prompt**:
   ```cmd
   set GITHUB_TOKEN=ghp_your-token-here
   ```

   **Linux/macOS**:
   ```bash
   export GITHUB_TOKEN="ghp_your-token-here"
   ```

   **Persistent (add to profile)**:
   - Windows: Add to PowerShell profile or system environment variables
   - Linux/macOS: Add to `~/.bashrc` or `~/.zshrc`

#### Step 3: Frontend Setup

**Option A: Direct File Access** (Simplest):
```powershell
cd Frontend
Start-Process "index.html"
```

**Option B: Python HTTP Server**:
```bash
cd Frontend
python -m http.server 3000
# Open http://localhost:3000
```

**Option C: Node.js HTTP Server**:
```bash
npm install -g http-server
cd Frontend
http-server -p 3000
# Open http://localhost:3000
```

**Option D: VS Code Live Server**:
1. Install "Live Server" extension in VS Code
2. Right-click `index.html`
3. Select "Open with Live Server"

#### Step 4: Verify Installation

1. **Backend Check**:
   ```powershell
   # Backend should show:
   ✓ HTTP Server configured with endpoints
   ✓ CORS enabled for frontend access
   ✓ Ready to accept requests at http://localhost:8080
   ```

2. **Frontend Check**:
   - Open in browser
   - You should see the Budgeteer interface with Budgie mascot
   - Try a basic search (e.g., "milk")

3. **AI Feature Check**:
   - Switch to "AI-Powered" search mode
   - Try: "I need party snacks under $30"
   - Should return relevant items

---

## Backend Documentation

### Project Structure

```
Backend/
├── include/               # Header files
│   ├── ApiServer.h       # HTTP server and request routing
│   ├── Database.h        # CSV data loading and queries
│   ├── Item.h            # Product data model
│   ├── LLMInterface.h    # AI/NLP integration
│   └── StoreApiClient.h  # Store search abstraction
├── src/                  # Implementation files
│   ├── ApiServer.cpp
│   ├── Database.cpp
│   ├── Item.cpp
│   ├── LLMInterface.cpp
│   ├── StoreApiClient.cpp
│   └── main.cpp          # Entry point
├── SampleDataset/        # Data files
│   └── yec_competition_dataset.csv
├── build/                # Build output (generated)
├── CMakeLists.txt        # Build configuration
└── README.md
```

### Core Classes

#### 1. Item Class
**Purpose**: Represents a product with pricing information

**Key Properties**:
- `itemId`: Unique identifier
- `itemName`: Product name
- `itemDescription`: Product description
- `currentPrice`: Current price
- `store`: Store name (Walmart, Loblaws, Costco)
- `categoryTags`: Product categories
- `imageUrl`: Product image URL
- `priceDate`: Date of price recording

**Key Methods**:
```cpp
std::string toJson() const;              // Convert to JSON
bool hasCategory(const std::string& category) const;
```

#### 2. Database Class
**Purpose**: Handles CSV data loading and querying

**Key Methods**:
```cpp
bool loadFromCSV();                      // Load data from CSV file
std::vector<Item> getAllItems() const;   // Get all items
std::vector<Item> getItemById(int id) const;
std::vector<Item> getItemsByName(const std::string& name) const;
std::vector<Item> getItemsByStore(const std::string& store) const;
std::vector<Item> getItemsByCategory(const std::string& category) const;
std::vector<Item> getItemsByPriceRange(double min, double max) const;
std::vector<Item> searchItems(const std::string& term) const;
double getAveragePrice(int itemId) const;
double getMinPrice(int itemId) const;
double getMaxPrice(int itemId) const;
```

**Search Algorithm**:
- Exact match prioritization (200 points)
- Prefix matching (150 points)
- Contains matching (100 points)
- Levenshtein distance similarity (up to 60 points)
- Word-based partial matching (25-40 points)
- Score threshold: 15.0 minimum
- Result limit: Top 50 items

#### 3. StoreApiClient Class
**Purpose**: Abstracts store-specific search logic

**Key Methods**:
```cpp
std::vector<Item> searchWalmart(const std::string& query);
std::vector<Item> searchLoblaws(const std::string& query);
std::vector<Item> searchCostco(const std::string& query);
std::vector<Item> searchAllStores(const std::string& query);
std::vector<Item> comparePrices(const std::string& productName);
```

#### 4. LLMInterface Class
**Purpose**: Natural language processing and AI integration

**Query Processing Modes**:
```cpp
enum class Mode {
    CHEAPEST_MIX,      // Select cheapest per category
    SINGLE_STORE,      // Optimize for one store
    BUDGET_INSIGHT     // Provide budget analysis
};
```

**Key Methods**:
```cpp
// Main interface
std::string processNaturalLanguageQuery(const std::string& query, Mode mode);
std::vector<Item> generateShoppingList(const std::string& request);
std::string getBudgetInsight(const std::vector<Item>& items);

// Configuration
void setOpenAIKey(const std::string& key);
void enableGPTMode(bool enable);
void setGPTModel(const std::string& model);
void setDailyQueryLimit(int limit);

// Internal processing
std::string callGPTAPI(const std::string& prompt);
std::vector<Item> cherryPickRelevantItems(const std::string& query, 
                                          const std::vector<Item>& items);
std::vector<Item> refineShoppingListWithReasoning(const std::string& query,
                                                  std::vector<Item> initialItems,
                                                  int maxIterations);
std::vector<Item> validateFinalList(const std::string& query, 
                                    const std::vector<Item>& items);
```

**AI Features**:
1. **Intent Detection**: Identifies user intent (search, compare, shopping list, budget)
2. **Query Expansion**: Expands generic terms to specific products
3. **Cherry-Picking**: Filters results for relevance using GPT
4. **Reasoning**: Validates shopping list completeness
5. **Final Validation**: Removes obviously incorrect items
6. **Hybrid Mode**: Automatically chooses between local and GPT processing

#### 5. ApiServer Class
**Purpose**: HTTP server and request routing

**Key Methods**:
```cpp
bool initialize();                        // Load database
void startHttpServer();                   // Start HTTP server
void run();                              // Start CLI mode

// Request handlers
std::string handleGetAllItems() const;
std::string handleSearchItems(const std::string& term) const;
std::string handleNaturalLanguageQuery(const std::string& query);
std::string handleGenerateShoppingList(const std::string& request);
```

### Build System

#### CMake Configuration

The project uses CMake for cross-platform builds. Key features:

```cmake
cmake_minimum_required(VERSION 3.10)
project(BudgeteerAPI VERSION 1.0)

# C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Dependencies (header-only libraries fetched automatically)
FetchContent_Declare(httplib ...)
FetchContent_Declare(json ...)

# Executable
add_executable(BudgeteerAPI ...)
target_include_directories(BudgeteerAPI ...)
target_link_libraries(BudgeteerAPI ...)
```

#### Build Commands

```bash
# Configure
cmake -B build

# Build (Debug)
cmake --build build --config Debug

# Build (Release)
cmake --build build --config Release

# Clean
cmake --build build --target clean

# Rebuild
cmake --build build --clean-first
```

---

## Frontend Documentation

### Project Structure

```
Frontend/
├── index.html              # Main HTML file
├── styles.css             # All styles and animations
├── app.js                 # Application logic
├── shopping-list-styles.css  # Shopping list specific styles
├── app-enhanced.js        # Enhanced features (if used)
└── Assets/               # Images and media
    ├── budgie-2d.png     # Mascot image
    └── [other assets]
```

### HTML Structure

```html
<!DOCTYPE html>
<html lang="en">
<head>
  <!-- Meta tags, title, fonts, styles -->
</head>
<body>
  <!-- Hero Section -->
  <section class="hero">
    <div class="mascot-container">
      <img src="Assets/budgie-2d.png" class="mascot-image">
    </div>
    <h1>Budgeteer</h1>
    <p>Smart Shopping, Better Savings</p>
  </section>

  <!-- Search Mode Toggle -->
  <div class="search-mode-toggle">
    <button class="mode-toggle-btn active" id="basicSearchToggle">
      Basic Search
    </button>
    <button class="mode-toggle-btn" id="aiSearchToggle">
      AI-Powered
    </button>
  </div>

  <!-- Basic Search Section -->
  <section class="basic-search-section">
    <!-- Search bar, filters, sort -->
  </section>

  <!-- AI Search Section -->
  <section class="ai-search-section hidden">
    <!-- AI search card -->
  </section>

  <!-- Stats Section -->
  <section class="stats-section">
    <!-- Database statistics -->
  </section>

  <!-- Results Section -->
  <section class="results-container hidden">
    <!-- Search results grid -->
  </section>

  <!-- Loading/Empty States -->
  <!-- Modal for item details -->
  
  <script src="app.js"></script>
</body>
</html>
```

### CSS Architecture

#### CSS Variables (Design Tokens)
```css
:root {
  /* Colors */
  --primary: #4F46E5;        /* Indigo */
  --secondary: #10B981;      /* Green */
  --error: #EF4444;          /* Red */
  --background: #F9FAFB;     /* Light gray */
  --surface: #FFFFFF;        /* White */
  --text-primary: #111827;   /* Near black */
  --text-secondary: #6B7280; /* Gray */
  
  /* Spacing */
  --spacing-xs: 0.5rem;
  --spacing-sm: 1rem;
  --spacing-md: 1.5rem;
  --spacing-lg: 2rem;
  --spacing-xl: 3rem;
  
  /* Typography */
  --font-family: 'Inter', sans-serif;
  --font-size-sm: 0.875rem;
  --font-size-base: 1rem;
  --font-size-lg: 1.125rem;
  --font-size-xl: 1.25rem;
  --font-size-2xl: 1.5rem;
  --font-size-3xl: 2rem;
  
  /* Effects */
  --shadow-sm: 0 1px 2px rgba(0,0,0,0.05);
  --shadow-md: 0 4px 6px rgba(0,0,0,0.1);
  --shadow-lg: 0 10px 15px rgba(0,0,0,0.1);
  --radius-sm: 0.375rem;
  --radius-md: 0.5rem;
  --radius-lg: 0.75rem;
  --transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
}
```

#### Key CSS Classes

**Layout**:
- `.container`: Max-width wrapper with padding
- `.hero`: Hero section with gradient background
- `.search-section`: Search interface container
- `.results-grid`: CSS Grid layout for results

**Components**:
- `.mascot-container`: Animated mascot container
- `.search-mode-toggle`: Mode switcher buttons
- `.ai-search-card`: AI search interface card
- `.item-card`: Individual product card
- `.modal`: Overlay modal for details

**Utilities**:
- `.hidden`: Display none
- `.loading`: Loading animation
- `.error`: Error state styling

### JavaScript Architecture

#### State Management

```javascript
const state = {
    items: [],                    // All loaded items
    filteredItems: [],           // Currently filtered items
    stores: [],                  // Available stores
    categories: [],              // Available categories
    currentSearch: '',           // Current search term
    currentFilters: {            // Active filters
        store: '',
        category: '',
        minPrice: null,
        maxPrice: null
    }
};
```

#### Key Functions

**Initialization**:
```javascript
async function initializeApp()
function populateFilters()
function setupEventListeners()
```

**Search & Filtering**:
```javascript
function performSearch()           // Basic search
function performAISearch()         // AI-powered search
function applyFilters()            // Apply filter criteria
function sortResults()             // Sort results
```

**UI Management**:
```javascript
function displayResults(items)
function createItemCard(item)
function showItemDetail(item)
function showLoadingState()
function showEmptyState(message)
```

**API Integration**:
```javascript
async function callBackendAPI(endpoint, options)
async function fetchSearchResults(query)
async function fetchAISearchResults(query)
```

---

## API Reference

### REST API Endpoints

Base URL: `http://localhost:8080`

#### 1. Root Endpoint
```http
GET /
```

**Response**:
```json
{
  "message": "Budgeteer API Server",
  "version": "1.0",
  "status": "running"
}
```

#### 2. Search Items
```http
GET /search?q={query}
```

**Parameters**:
- `q` (required): Search query string

**Example Request**:
```http
GET /search?q=milk
```

**Response**:
```json
{
  "success": true,
  "count": 15,
  "items": [
    {
      "item_id": 1001,
      "item_name": "2% Milk (2L)",
      "item_description": "Fresh 2% milk",
      "current_price": 4.99,
      "store": "Walmart",
      "category_tags": ["dairy", "beverages"],
      "image_url": "https://example.com/milk.jpg",
      "price_date": "2024-01-15"
    }
  ]
}
```

#### 3. Get All Items
```http
GET /items
```

**Response**: Same structure as search, returns all items.

#### 4. Get Item by ID
```http
GET /items/{id}
```

**Parameters**:
- `id` (required): Item ID

**Example**:
```http
GET /items/1001
```

#### 5. Get Stores
```http
GET /stores
```

**Response**:
```json
{
  "success": true,
  "stores": ["Walmart", "Loblaws", "Costco"]
}
```

#### 6. Get Categories
```http
GET /categories
```

**Response**:
```json
{
  "success": true,
  "categories": ["dairy", "beverages", "electronics", "baby", "household"]
}
```

#### 7. Get Item Statistics
```http
GET /items/{id}/stats
```

**Response**:
```json
{
  "success": true,
  "item_id": 1001,
  "statistics": {
    "average_price": 5.25,
    "min_price": 4.99,
    "max_price": 5.99
  }
}
```

#### 8. Natural Language Query
```http
POST /api/llm/query
Content-Type: application/json
```

**Request Body**:
```json
{
  "query": "I need party snacks under $30"
}
```

**Response**:
```json
{
  "success": true,
  "query": "I need party snacks under $30",
  "response": "Here are the cheapest options across all stores:\n\n| Store | Item | Price |\n|-------|------|-------|\n| Walmart | Chips | $2.99 |\n..."
}
```

#### 9. Generate Shopping List
```http
POST /api/llm/shopping-list
Content-Type: application/json
```

**Request Body**:
```json
{
  "prompt": "ingredients for chocolate cake"
}
```

**Response**:
```json
{
  "success": true,
  "shopping_list": {
    "item_count": 6,
    "total_cost": 28.75,
    "items": [
      {
        "item_id": 1001,
        "item_name": "All-Purpose Flour (5kg)",
        "current_price": 7.99,
        "store": "Walmart"
      }
    ]
  }
}
```

#### 10. Real-time Store Search
```http
GET /api/realtime/search?q={query}
```

**Note**: Currently uses database; real-time APIs removed.

#### 11. Compare Prices
```http
GET /api/realtime/compare?product={product}
```

**Response**: Returns items sorted by price (lowest first).

---

## Features

### 1. Basic Search Mode

**What it does**:
- Traditional keyword-based product search
- Advanced filtering by store, category, and price range
- Sorting options (relevance, price, name)
- Instant results from database

**How to use**:
1. Enter product name in search box
2. Click "Search" or press Enter
3. Use filters to narrow results
4. Click any item for detailed view

**Search Algorithm**:
- Exact match gets highest priority
- Prefix matching (starts with query)
- Contains matching (query appears anywhere)
- Fuzzy matching (similar spellings)
- Word-based partial matching

**Example Searches**:
- "milk" → Returns all milk products
- "samsung tv" → Returns Samsung TVs
- "dish soap" → Returns dish soaps
- "baby shampoo" → Returns baby care products

### 2. AI-Powered Search Mode

**What it does**:
- Natural language query understanding
- Budget-aware recommendations
- Shopping list generation
- Context-aware product selection

**How to use**:
1. Click "AI-Powered" toggle
2. Describe what you need in natural language
3. Click "Search with AI"
4. View curated results

**Example Queries**:
- "I need party snacks under $30"
- "Find me ingredients to bake a cake"
- "Weekly groceries for family of 4"
- "Affordable breakfast items"
- "Cleaning supplies for apartment"

**AI Processing Pipeline**:
1. **Intent Analysis**: Understands user's goal
2. **Product Expansion**: Identifies specific products needed
3. **Database Search**: Searches for relevant items
4. **Cherry-Picking**: GPT filters for relevance
5. **Reasoning**: Validates completeness (for recipes, etc.)
6. **Final Validation**: Removes obviously wrong items
7. **Result Ranking**: Optimizes for budget/convenience

### 3. Price History

**What it does**:
- Shows historical prices for each item
- Calculates min, max, and average prices
- Identifies best time to buy

**How to use**:
1. Click any product card
2. View price history modal
3. See statistics and trends

**Information shown**:
- Current price (highlighted)
- Average price across all dates
- Lowest recorded price (green)
- Highest recorded price (red)
- Chronological price list with dates

### 4. Budget Insights

**What it does**:
- Analyzes shopping lists for cost optimization
- Compares single-store vs multi-store shopping
- Suggests savings opportunities

**How to access**:
```http
POST /api/llm/budget-insight
Body: { "items": [...] }
```

**Response includes**:
- Total item count
- Average price per item
- Cheapest single-store option
- Potential savings amount

### 5. Shopping List Generation

**What it does**:
- Creates shopping lists from natural language
- Suggests appropriate quantities
- Optimizes for budget constraints
- Ensures completeness (e.g., all cake ingredients)

**Examples**:
- Input: "ingredients for chocolate cake"
  - Output: flour, sugar, cocoa powder, eggs, butter, milk, baking powder, vanilla

- Input: "party snacks under $50"
  - Output: chips, dip, cookies, soda, pretzels, candy

- Input: "weekly breakfast items"
  - Output: eggs, bread, milk, cereal, butter, orange juice

**Processing Steps**:
1. GPT analyzes request
2. Generates product list
3. Searches database for each item
4. Selects best options (cheapest or specific store)
5. Validates logical completeness
6. Returns organized list with prices

### 6. Multi-Store Comparison

**What it does**:
- Compares same product across stores
- Shows price differences
- Highlights best deal

**How to use**:
1. Search for a product
2. View results from multiple stores
3. Compare prices side-by-side

**Ranking Modes**:
- **Cheapest Mix**: Best price per product (may involve multiple stores)
- **Single Store**: Best overall deal at one store (convenience)
- **Specific Store**: Filter by preferred store

### 7. Advanced Filtering

**Filter Options**:
- **Store**: Walmart, Loblaws, Costco
- **Category**: Electronics, Food, Baby, Household, etc.
- **Price Range**: Min and Max price sliders
- **Sort By**: Relevance, Price (Low-High), Price (High-Low), Name (A-Z)

**How filters work**:
- Applied on top of search results
- Multiple filters can be combined
- Real-time result updates
- Clear indicators when filters active

### 8. Responsive Design

**Breakpoints**:
- Desktop: 1200px+
- Tablet: 768px - 1199px
- Mobile: < 768px

**Mobile Optimizations**:
- Collapsible filters
- Simplified layout
- Touch-friendly buttons
- Optimized image loading
- Reduced animations

---

## Usage Guide

### For End Users

#### Getting Started

1. **Open the Application**
   - Navigate to Budgeteer website or open `index.html`
   - You'll see the hero section with Budgie mascot

2. **Choose Search Mode**
   - **Basic Search**: For simple product lookups
   - **AI-Powered**: For complex requests or shopping lists

#### Basic Search Workflow

1. **Enter Search Term**
   ```
   Example: "milk"
   ```

2. **Review Results**
   - Products appear in grid layout
   - Each card shows: name, price, store, description

3. **Apply Filters** (optional)
   - Store: Select specific store
   - Category: Choose product category
   - Price Range: Set min/max prices

4. **Sort Results** (optional)
   - Relevance (default)
   - Price: Low to High
   - Price: High to Low
   - Name: A-Z

5. **View Details**
   - Click any product card
   - See complete price history
   - View statistics (min, max, average)
   - Compare across dates

#### AI Search Workflow

1. **Switch to AI Mode**
   - Click "AI-Powered" toggle button

2. **Describe Your Needs**
   ```
   Examples:
   - "I need party snacks under $30"
   - "Find ingredients to bake a chocolate cake"
   - "Weekly groceries for family of 4"
   ```

3. **Submit Query**
   - Click "Search with AI" button
   - Or press Shift+Enter in the textarea

4. **Review AI Results**
   - Budgie curates relevant products
   - Results optimized for your budget
   - Complete shopping lists for recipes

5. **Refine if Needed**
   - Add more details to your query
   - Specify budget constraints
   - Mention preferred stores

#### Tips for Best Results

**Basic Search**:
- Use specific product names
- Try different variations ("pop" vs "soda")
- Use filters to narrow results
- Check multiple stores for best price

**AI Search**:
- Be conversational
- Mention budget if important
- Specify quantity/servings
- Include context (party, dinner, etc.)

**Price Comparison**:
- Click items to see price history
- Look for lowest historical price
- Check price trends
- Consider store proximity vs savings

### For Developers

#### Setting Up Development Environment

1. **Clone Repository**
   ```bash
   git clone https://github.com/YK12321/Budgeteer.git
   cd Budgeteer
   ```

2. **Backend Setup**
   ```bash
   cd Backend
   cmake -B build
   cmake --build build --config Debug
   ```

3. **Configure GitHub Token**
   ```bash
   $env:GITHUB_TOKEN="ghp_your-token-here"
   ```

4. **Run Backend**
   ```bash
   .\build\bin\Debug\BudgeteerAPI.exe --http --port 8080
   ```

5. **Frontend Setup**
   ```bash
   cd Frontend
   # Open index.html or use local server
   ```

#### Making Code Changes

**Backend Changes**:
1. Edit source files in `Backend/src/` or `Backend/include/`
2. Rebuild: `cmake --build build --config Debug`
3. Restart server
4. Test endpoints with Postman or frontend

**Frontend Changes**:
1. Edit `index.html`, `styles.css`, or `app.js`
2. Refresh browser (no build step needed)
3. Check console for errors
4. Test on multiple screen sizes

#### Adding New Features

**New API Endpoint**:
```cpp
// In ApiServer.cpp
svr.Get("/api/new-endpoint", [this](const httplib::Request& req, httplib::Response& res) {
    // Implementation
    std::string response = handleNewFeature();
    res.set_content(response, "application/json");
});
```

**New Frontend Component**:
```javascript
// In app.js
function createNewComponent(data) {
    const element = document.createElement('div');
    element.className = 'new-component';
    element.innerHTML = `...`;
    return element;
}
```

**New CSS Style**:
```css
/* In styles.css */
.new-component {
    /* styles */
}
```

#### Debugging

**Backend**:
```cpp
// Add debug output
std::cout << "[DEBUG] Variable value: " << variable << std::endl;

// Use debugger
// Visual Studio: Set breakpoints and press F5
// GDB: gdb ./BudgeteerAPI
```

**Frontend**:
```javascript
// Console logging
console.log('Debug info:', variable);
console.table(arrayOfObjects);

// Breakpoints
debugger; // Browser will pause here

// Chrome DevTools
// F12 → Sources → Set breakpoints
```

#### Testing

**Backend Testing**:
```bash
# Manual testing with curl
curl http://localhost:8080/items
curl -X POST http://localhost:8080/api/llm/query \
     -H "Content-Type: application/json" \
     -d '{"query":"test"}'
```

**Frontend Testing**:
- Open browser DevTools (F12)
- Check Network tab for API calls
- Verify Console for errors
- Test on different browsers
- Test responsive design

---

## Troubleshooting

### Common Issues

#### Backend Won't Start

**Issue**: Port 8080 already in use
```
Error: Failed to start HTTP server on port 8080
Port may already be in use
```

**Solutions**:
1. Check if another instance is running:
   ```powershell
   netstat -ano | findstr :8080
   ```
2. Kill the process or use different port:
   ```bash
   .\BudgeteerAPI.exe --http --port 3000
   ```

**Issue**: CSV file not found
```
Error: Could not open file SampleDataset/yec_competition_dataset.csv
```

**Solutions**:
1. Verify file exists in correct location
2. Check working directory
3. Use absolute path in Database constructor

**Issue**: OpenSSL missing (SSL errors)
```
Error: SSL connect error
```

**Solutions**:
1. Install OpenSSL:
   - Windows: Download from https://slproweb.com/products/Win32OpenSSL.html
   - Linux: `sudo apt-get install libssl-dev`
   - macOS: `brew install openssl`
2. Add to PATH or CMake configuration

#### Frontend Issues

**Issue**: CORS errors
```
Access to fetch at 'http://localhost:8080' from origin 'null' 
has been blocked by CORS policy
```

**Solutions**:
1. Backend must have CORS headers (already configured)
2. Use local server instead of file://
3. Check backend is running

**Issue**: API calls fail
```
Failed to fetch
TypeError: NetworkError when attempting to fetch resource
```

**Solutions**:
1. Verify backend is running
2. Check `API_BASE_URL` in app.js
3. Test endpoint directly: `http://localhost:8080/items`
4. Check browser console for details

**Issue**: Mascot image not showing
```
GET file:///Assets/budgie-2d.png 404 (Not Found)
```

**Solutions**:
1. Verify image exists in `Frontend/Assets/`
2. Check path in HTML
3. Use relative path, not absolute

#### AI Features Issues

**Issue**: AI search returns empty
```
{
  "success": true,
  "query": "...",
  "response": "No items found"
}
```

**Solutions**:
1. Check GitHub token is set:
   ```powershell
   echo $env:GITHUB_TOKEN
   ```
2. Verify token has correct permissions
3. Check internet connection
4. Try fallback to basic search
5. Check daily query limit

**Issue**: Rate limit exceeded
```
[LLM] Daily query limit reached (1000)
```

**Solutions**:
1. Increase limit:
   ```cpp
   llmInterface->setDailyQueryLimit(5000);
   ```
2. Wait for reset (24 hours)
3. Use hybrid mode (auto-fallback to local)

**Issue**: GPT returns irrelevant results
```
AI suggests electronics when user wants food
```

**Solutions**:
1. Improve query specificity
2. Check validation prompts
3. Cherry-picking threshold may be too loose
4. Verify final validation is enabled

### Debugging Tips

#### Enable Verbose Logging

**Backend**:
```cpp
// In main.cpp or LLMInterface.cpp
std::cout << "[DEBUG] Starting request processing..." << std::endl;
std::cout << "[DEBUG] Query: " << query << std::endl;
std::cout << "[DEBUG] Results count: " << results.size() << std::endl;
```

**Frontend**:
```javascript
// In app.js
const DEBUG = true;

function debug(message, data) {
    if (DEBUG) {
        console.log(`[DEBUG] ${message}`, data);
    }
}
```

#### Check API Responses

**Using Browser**:
1. Open DevTools (F12)
2. Go to Network tab
3. Perform action
4. Click on request
5. View Response tab

**Using curl**:
```bash
# Test search endpoint
curl -v http://localhost:8080/search?q=milk

# Test AI endpoint
curl -v -X POST http://localhost:8080/api/llm/query \
     -H "Content-Type: application/json" \
     -d '{"query":"test query"}'
```

**Using Postman**:
1. Create new request
2. Set URL: `http://localhost:8080/search?q=milk`
3. Click Send
4. View response

#### Verify Data Pipeline

1. **Check CSV Loading**:
   - Backend should log: "Successfully loaded X items from database"
   - If 0 items, check CSV file path

2. **Test Database Queries**:
   ```cpp
   auto items = database->getAllItems();
   std::cout << "Total items: " << items.size() << std::endl;
   ```

3. **Verify Search Algorithm**:
   ```cpp
   auto results = database->searchItems("milk");
   std::cout << "Search results: " << results.size() << std::endl;
   ```

4. **Check API Responses**:
   - Open http://localhost:8080/items in browser
   - Should see JSON with all items

### Performance Issues

**Backend Slow**:
- Check database size (CSV line count)
- Profile search algorithm
- Optimize query filters
- Consider caching frequent searches

**Frontend Slow**:
- Check number of DOM elements
- Use virtual scrolling for long lists
- Optimize images (lazy loading)
- Debounce search input

**AI Queries Slow**:
- Normal for GPT calls (200-500ms)
- Use loading indicators
- Consider caching responses
- Implement request timeouts

### Build Issues

**CMake Configuration Failed**:
```
CMake Error: Could not find compiler
```

**Solutions**:
- Install C++ compiler (MSVC, GCC, or Clang)
- Add compiler to PATH
- Specify generator: `cmake -G "Visual Studio 16 2019"`

**Linker Errors**:
```
undefined reference to `Database::loadFromCSV()'
```

**Solutions**:
- Ensure all .cpp files are included in CMakeLists.txt
- Clean and rebuild: `cmake --build build --clean-first`
- Check for missing dependencies

**Header Not Found**:
```
fatal error: httplib.h: No such file or directory
```

**Solutions**:
- FetchContent should download automatically
- Check internet connection
- Manually download and add to include path

---

## Performance & Optimization

### Backend Optimizations

#### 1. Search Algorithm
**Current Implementation**:
- O(n) linear scan with early filtering
- Score threshold: 15.0 (skip low-scoring items)
- Result limit: Top 50 items
- Token-efficient for AI processing

**Optimization Techniques**:
```cpp
// Early filtering with score threshold
if (score > MIN_SCORE_THRESHOLD) {
    scoredItems.push_back({item, score});
}

// Limit results
if (count >= MAX_RESULTS) {
    break;
}
```

**Future Improvements**:
- Implement inverted index for O(1) keyword lookup
- Cache frequent search terms
- Pre-compute similarity matrices
- Use trie data structure for prefix matching

#### 2. Database Loading
**Current**: Load all at startup (one-time cost)
**Optimization**: 
- Memory mapping for large files
- Incremental loading
- Binary format instead of CSV

#### 3. JSON Serialization
**Current**: Manual string concatenation
**Optimization**:
- Use nlohmann/json library throughout
- Stream serialization for large responses
- Compression for network transfer

#### 4. AI Request Optimization
**Implemented**:
- Hybrid mode (local for simple queries)
- Cherry-picking limits (50 products max)
- Result filtering before GPT call
- Daily query limits
- Request caching (planned)

**Token Usage**:
- Average query: ~500 tokens
- Cherry-picking: ~800 tokens
- Reasoning: ~600 tokens
- Total per shopping list: ~2000 tokens
- Cost: ~$0.0003 per shopping list

### Frontend Optimizations

#### 1. Initial Load
**Current**:
- ~15KB total (HTML + CSS + JS)
- No external dependencies
- Inline critical CSS
- Defer non-critical JS

**Metrics**:
- First Contentful Paint: <1s
- Time to Interactive: <1.5s
- Total Bundle Size: 15KB

#### 2. Runtime Performance
**Optimizations**:
```javascript
// Debounced search input
let searchTimeout;
searchInput.addEventListener('input', () => {
    clearTimeout(searchTimeout);
    searchTimeout = setTimeout(performSearch, 300);
});

// Virtual scrolling for long lists (future)
function renderVisibleItems() {
    const start = Math.floor(scrollTop / itemHeight);
    const end = Math.ceil((scrollTop + viewportHeight) / itemHeight);
    // Render only visible items
}
```

#### 3. CSS Animations
- Use CSS transforms (GPU-accelerated)
- Avoid layout thrashing
- RequestAnimationFrame for JS animations

```css
/* GPU-accelerated animation */
.mascot-image {
    animation: float 3s ease-in-out infinite;
}

@keyframes float {
    0%, 100% { transform: translateY(0) scale(1); }
    50% { transform: translateY(-20px) scale(1.05); }
}
```

#### 4. Image Optimization
- Lazy loading (future):
```javascript
img.loading = "lazy";
```
- Responsive images:
```html
<img srcset="image-320w.jpg 320w,
             image-640w.jpg 640w,
             image-1280w.jpg 1280w"
     sizes="(max-width: 768px) 100vw, 50vw"
     src="image-640w.jpg">
```

### Network Optimization

#### 1. HTTP/2
**Benefits**:
- Multiplexing (multiple requests per connection)
- Header compression
- Server push

**Implementation**:
- Use HTTP/2 compatible server
- Enable in cpp-httplib (if supported)

#### 2. Compression
```cpp
// Enable gzip compression
svr.set_post_routing_handler([](const Request& req, Response& res) {
    if (res.body.size() > 1024) {
        // Compress response
    }
});
```

#### 3. Caching
**Client-side**:
```javascript
// Cache API responses
const cache = new Map();
async function fetchWithCache(url) {
    if (cache.has(url)) return cache.get(url);
    const response = await fetch(url);
    cache.set(url, response);
    return response;
}
```

**Server-side**:
```cpp
// Cache frequent queries
std::map<std::string, std::vector<Item>> queryCache;

std::vector<Item> searchWithCache(const std::string& query) {
    if (queryCache.count(query)) {
        return queryCache[query];
    }
    auto results = database->searchItems(query);
    queryCache[query] = results;
    return results;
}
```

### Database Optimization

#### 1. Indexing
**Future Implementation**:
```cpp
// Index by item ID
std::unordered_map<int, Item> itemIndex;

// Index by store
std::unordered_map<std::string, std::vector<Item*>> storeIndex;

// Index by category
std::unordered_map<std::string, std::vector<Item*>> categoryIndex;
```

#### 2. Query Planning
- Filter before search (reduce dataset)
- Use indexes when available
- Parallel processing for independent queries

---

## Security

### Current Security Measures

#### 1. CORS Configuration
**Backend**:
```cpp
svr.set_default_headers({
    {"Access-Control-Allow-Origin", "*"},
    {"Access-Control-Allow-Methods", "GET, POST, OPTIONS"},
    {"Access-Control-Allow-Headers", "Content-Type"}
});
```

**Production Recommendation**:
```cpp
// Restrict to specific origins
{"Access-Control-Allow-Origin", "https://yourdomain.com"}
```

#### 2. API Key Management
**Current**: Environment variables
```powershell
$env:GITHUB_TOKEN="ghp_your-token-here"
```

**Security Best Practices**:
- Never commit API keys to repository
- Use `.gitignore` for `.env` files
- Rotate keys every 90 days
- Use separate keys for dev/prod
- Monitor usage dashboard

#### 3. Input Validation
**Current Implementation**:
```cpp
// Validate search term
if (req.has_param("q")) {
    std::string query = req.get_param_value("q");
    // Process query
} else {
    res.set_content(createErrorResponse("Missing query parameter"), 
                   "application/json");
}
```

**Recommended Improvements**:
```cpp
// Sanitize input
std::string sanitize(const std::string& input) {
    // Remove SQL injection attempts
    // Escape special characters
    // Limit length
    return cleanedInput;
}
```

#### 4. Rate Limiting
**Current**: Daily query limits for AI
```cpp
llmInterface->setDailyQueryLimit(1000);
```

**Production Recommendations**:
- Per-IP rate limiting
- Per-user rate limiting
- Gradual backoff for violations
- CAPTCHA for suspicious activity

### Security Checklist

#### Development Phase
- [ ] API keys in environment variables
- [ ] `.env` added to `.gitignore`
- [ ] No secrets in code
- [ ] Input validation on all endpoints
- [ ] Error messages don't expose internals

#### Pre-Production
- [ ] HTTPS enabled (SSL/TLS)
- [ ] CORS restricted to known origins
- [ ] Rate limiting implemented
- [ ] SQL injection prevention (if using SQL)
- [ ] XSS prevention in frontend
- [ ] CSRF tokens for state-changing operations

#### Production
- [ ] Security headers (HSTS, CSP, X-Frame-Options)
- [ ] Regular security audits
- [ ] Dependency updates
- [ ] Monitoring and logging
- [ ] Incident response plan

### Common Vulnerabilities & Mitigations

#### 1. SQL Injection
**Risk**: N/A (using CSV, not SQL database)
**Future**: If migrating to SQL, use prepared statements

#### 2. XSS (Cross-Site Scripting)
**Risk**: Frontend displays user-generated content
**Mitigation**:
```javascript
// Sanitize before inserting into DOM
function escapeHtml(unsafe) {
    return unsafe
        .replace(/&/g, "&amp;")
        .replace(/</g, "&lt;")
        .replace(/>/g, "&gt;")
        .replace(/"/g, "&quot;")
        .replace(/'/g, "&#039;");
}
```

#### 3. CSRF (Cross-Site Request Forgery)
**Risk**: State-changing operations without CSRF tokens
**Mitigation**:
```javascript
// Include CSRF token in POST requests
const csrfToken = document.querySelector('meta[name="csrf-token"]').content;

fetch('/api/endpoint', {
    method: 'POST',
    headers: {
        'X-CSRF-Token': csrfToken,
        'Content-Type': 'application/json'
    },
    body: JSON.stringify(data)
});
```

#### 4. API Key Exposure
**Risk**: GitHub token visible in client-side code
**Mitigation**: 
- Never send API keys to frontend
- All AI calls must go through backend
- Backend validates and forwards requests

#### 5. Denial of Service (DoS)
**Risk**: Unlimited requests overwhelm server
**Mitigation**:
- Rate limiting (requests per IP/user)
- Request size limits
- Timeout configuration
- Load balancing (production)

---

## Appendix

### Glossary

- **API**: Application Programming Interface
- **CORS**: Cross-Origin Resource Sharing
- **CSV**: Comma-Separated Values
- **GPT**: Generative Pre-trained Transformer
- **HTTP**: Hypertext Transfer Protocol
- **JSON**: JavaScript Object Notation
- **LLM**: Large Language Model
- **NLP**: Natural Language Processing
- **REST**: Representational State Transfer
- **SSL/TLS**: Secure Sockets Layer / Transport Layer Security

### Useful Resources

#### Official Documentation
- [C++ Reference](https://en.cppreference.com/)
- [CMake Documentation](https://cmake.org/documentation/)
- [MDN Web Docs](https://developer.mozilla.org/)
- [GitHub Models API](https://docs.github.com/en/github-models)

#### Libraries
- [cpp-httplib](https://github.com/yhirose/cpp-httplib)
- [nlohmann/json](https://github.com/nlohmann/json)
- [OpenSSL](https://www.openssl.org/)

#### Tools
- [Postman](https://www.postman.com/) - API testing
- [VS Code](https://code.visualstudio.com/) - Code editor
- [Chrome DevTools](https://developer.chrome.com/docs/devtools/) - Browser debugging

### Change Log

#### Version 1.0 (October 2025)
- Initial release
- Basic search functionality
- AI-powered search with GPT-4o-mini
- Multi-store price comparison
- Shopping list generation
- Price history tracking
- Responsive web interface

#### Planned Features (Future Versions)
- User authentication
- Favorites/wishlists
- Price drop notifications
- Mobile app (React Native)
- Real-time store API integration
- Barcode scanner
- Store location maps
- Export to PDF
- Email notifications

### Contributing

We welcome contributions! See `CONTRIBUTING.md` for guidelines.

### License

MIT License - See `LICENSE` file for details.

### Support

For questions or issues:
- Open an issue on GitHub
- Email: [support email]
- Documentation: This file

---

**Built with ❤️ for YEC 2025**

