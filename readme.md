# Budgeteer 🛒💰

**An intelligent price comparison platform helping families save money on groceries**

🌐 **Live Demo:** [https://yk12321.github.io/Budgeteer/](https://yk12321.github.io/Budgeteer/)

---

## Team Info

**Team Name:** Budgeteer  
**Team Members:** Terry Liu, YK  
**Competition:** York Entrepreneurship Competition (YEC) 2025  
**Project Title:** Budgeteer - Smart Price Comparison Platform

---

## What It Does

Our solution to the problem of inflation is to create an app that takes in an item the user wants to buy and finds the lowest-cost shop to buy it. The user can also enter a list of shopping items manually or by our AI, and the app will find the store that will give the cheapest cost to buy all the items.

### Core Features

1. **Single Item Search**: Enter any grocery item to instantly compare prices across Walmart, Loblaws, and Costco, with historical price data and filtering options
2. **AI Shopping List Generator**: Use natural language prompts (e.g., "I need snacks for a party under $50") to automatically generate optimized shopping lists
3. **Manual Shopping List Builder**: Add items one-by-one to create custom shopping lists, then find which store offers the lowest total cost
4. **Price History Tracking**: View historical pricing data to identify the best time to buy and speculate future prices
5. **Advanced Filtering**: Filter by specific stores, item categories, or budget range to refine your search

---

## How To Run It

### Prerequisites

- **Backend**: CMake 3.10+, C++17 compiler (MSVC/GCC), OpenSSL
- **Frontend**: Any modern web browser
- **AI Features**: GitHub Personal Access Token (for GPT integration)

### Quick Start (30 Seconds)

#### 1. Setup Backend API

```bash
# Navigate to backend directory
cd Backend

# Build the project
cmake -B build
cmake --build build --config Debug

# Set up your GitHub token (for AI features)
# Create Backend/.env file with: GITHUB_TOKEN=your_token_here

# Run the server
.\build\bin\Debug\BudgeteerAPI.exe --http --port 8080
```

#### 2. Setup Frontend

```bash
# Navigate to frontend directory
cd Frontend

# Open in browser (PowerShell)
Start-Process index.html
```

#### 3. Start Using Budgeteer

**Single Item Search:**
- Simply enter the item you want to buy in the search feature
- The app will take your item and display it across our three selected databases: Loblaws, Walmart, and Costco
- It will display the cheapest store, along with the past data for the item
- Use the filter options to choose specific stores, items, or budget range

**AI Shopping List:**
- Switch to "AI Search Mode" 
- Enter a natural language prompt, for example: "I need snacks for a party under $50"
- The AI will generate a shopping list of items to buy
- Compare total costs across stores

**Manual Shopping List:**
- Type the item into the manual search bar
- Click "Add Item" to add one item to the shopping list
- Repeat this multiple times to generate your own shopping list
- System finds the cheapest store for your complete list

### Detailed Setup

See [DOCUMENTATION.md](DOCUMENTATION.md) for platform-specific instructions, troubleshooting, and advanced configuration options.

---

## Tech Choices

### Backend: C++17
**Why?** Performance and efficiency. C++ provides near-native speed for processing thousands of database records and handling multiple concurrent API requests. For a price comparison platform where response time directly impacts user experience, C++ gives us sub-millisecond search performance.

### HTTP Server: cpp-httplib
**Why?** Lightweight, header-only library requiring no external dependencies. Provides clean REST API implementation with built-in CORS support, perfect for rapid prototyping while maintaining production-level performance.

### Frontend: HTML, CSS, and JavaScript
**Why?** We chose HTML, CSS, and JavaScript for the frontend to create a simple, accessible, and responsive user interface. Zero dependencies mean zero overhead, resulting in faster load times and better performance on low-end devices. This accessibility focus ensures our solution reaches families who need it most.

### AI Integration: GPT-4o-mini (via GitHub Models API)
**Why?** We used GPT-4o-mini for the AI system running the app because it is cheap and efficient. GPT-4o-mini offers strong natural language understanding at a fraction of the cost of larger models. Our hybrid approach combines local processing for basic searches with AI for complex queries, optimizing both cost and performance.

### Database: CSV with Custom Search Algorithm
**Why?** Given the competition dataset format, we built an intelligent search engine using Levenshtein distance for fuzzy matching. This allows users to find "organization milk" even when searching for "organic milk", significantly improving usability.

---

## Impact

### Helping Families Save Money

Families that want to shop cheaply can find the cheapest prices for shopping, and they can budget and save money. The app is also useful for families to speculate future prices of items, and to find times where the prices are the cheapest.

**Real-World Scenario:**  
A family of four spending $150/week on groceries could save $15-30 (10-20%) by shopping at the optimal store for their specific items. That's **$780-1,560 saved annually** - equivalent to a month or two of groceries.

### Community Benefits

1. **Budget Empowerment**: Low-income families gain transparency into pricing, enabling informed decisions
2. **Time Savings**: Eliminates the need to visit multiple stores or manually compare prices
3. **Price Awareness**: Historical data helps users identify sales patterns and optimal buying times to purchase items
4. **Price Speculation**: Users can analyze historical trends to predict when prices will be lowest
5. **Reduced Food Insecurity**: Better budgeting means families can afford more nutritious food

### Accessibility

- **No App Install Required**: Browser-based solution works on any device
- **Simple Interface**: Designed for all ages and technical skill levels
- **Offline Dataset**: Works with or without real-time store APIs
- **AI Optional**: Core functionality available without AI features

---

## Assumptions

### Data Assumptions

1. **Dataset Accuracy**: Product prices and information in the provided CSV dataset are accurate and represent real-world values
2. **Price Consistency**: Prices are consistent across all locations of the same store chain (e.g., all Walmart locations)
3. **Data Freshness**: Historical price data represents actual price changes over time, not duplicate entries
4. **Product Matching**: Items with the same name across different stores are comparable products
5. **Currency**: All prices are in Canadian dollars (CAD)

### User Assumptions

1. **Item Availability**: We assumed that the user's item can be found in the shops. If they do not, then the app will simply not display anything
2. **Internet Access**: Users have basic internet connectivity to access the web application
3. **Browser Compatibility**: Users have a modern web browser (Chrome, Firefox, Safari, Edge - released within last 2 years)
4. **Search Behavior**: Users can describe items in simple terms (e.g., "milk" rather than "Lactose-free 2% partly skimmed milk 2L")
5. **Shopping Lists**: Users shop for multiple items at once, making store comparison valuable

### Shopping Behavior Assumptions

1. **Single Store Preference**: We assumed that it would be cheaper for the user to buy all the items from the same store, than to travel to multiple stores
2. **Budget Constraints**: If the budget is too low, the AI system removes some items to meet the user's price constraints
3. **Price Sensitivity**: Users prioritize cost savings over brand loyalty or store preference

### Technical Assumptions

1. **Store APIs**: In production, stores would provide real-time API access or web scraping would be legally permissible
2. **AI API Availability**: GitHub Models API or equivalent OpenAI-compatible API remains accessible
3. **Dataset Structure**: Competition dataset follows the documented schema with consistent column names
4. **Scaling**: Current architecture handles the competition dataset size (~10,000 items); production deployment would require database optimization
5. **Security**: HTTPS deployment available for production; current demo runs on HTTP for simplicity

---

## References

### Libraries and Frameworks

[1] Y. Hirose, "cpp-httplib: A C++ header-only HTTP/HTTPS server and client library," GitHub repository, 2024. [Online]. Available: https://github.com/yhirose/cpp-httplib

[2] N. Lohmann, "JSON for Modern C++," GitHub repository, version 3.x, 2024. [Online]. Available: https://github.com/nlohmann/json

[3] OpenSSL Software Foundation, "OpenSSL: Cryptography and SSL/TLS Toolkit," version 3.x, 2024. [Online]. Available: https://www.openssl.org/

[4] Kitware, Inc., "CMake: Cross-platform build system," version 3.10+, 2024. [Online]. Available: https://cmake.org/

### APIs and Models

[5] GitHub, Inc., "GitHub Models API Documentation," 2024. [Online]. Available: https://github.com/marketplace/models

[6] OpenAI, "GPT-4o-mini: Language model for efficient text generation," 2024. [Online]. Available: https://platform.openai.com/docs/models

### Development Tools

[7] Microsoft Corporation, "Visual Studio Code," version 1.90+, 2024. [Online]. Available: https://code.visualstudio.com/

[8] GitHub, Inc., "GitHub Copilot: AI pair programmer," 2024. [Online]. Available: https://github.com/features/copilot

[9] Git Software Conservancy, "Git: Distributed version control system," 2024. [Online]. Available: https://git-scm.com/

### Algorithms and Techniques

[10] V. I. Levenshtein, "Binary codes capable of correcting deletions, insertions, and reversals," *Soviet Physics Doklady*, vol. 10, no. 8, pp. 707-710, 1966.

[11] R. T. Fielding, "Architectural Styles and the Design of Network-based Software Architectures," Ph.D. dissertation, University of California, Irvine, 2000. [REST architecture]

### Standards

[12] Internet Engineering Task Force (IETF), "RFC 9110: HTTP Semantics," 2022. [Online]. Available: https://www.rfc-editor.org/rfc/rfc9110

[13] ECMA International, "ECMA-404: The JSON Data Interchange Syntax," 2nd ed., 2017. [Online]. Available: https://www.ecma-international.org/publications-and-standards/standards/ecma-404/

[14] World Wide Web Consortium (W3C), "Cross-Origin Resource Sharing (CORS)," 2020. [Online]. Available: https://www.w3.org/TR/cors/

### Competition Resources

[15] York Entrepreneurship Competition (YEC), "Programming Competition Dataset," York University, 2025.

---

## Additional Documentation

- **[DOCUMENTATION.md](DOCUMENTATION.md)** - Complete technical documentation (460+ lines)
- **[CITATIONS.md](CITATIONS.md)** - Comprehensive list of all external resources used
- **[CODE_REFACTORING_SUMMARY.md](CODE_REFACTORING_SUMMARY.md)** - Code quality improvements and architecture overview

---

## License

This project was created for the York Entrepreneurship Competition 2025.

---

## 📁 Project Structure

```
Programming/
├── Backend/                    # C++ Backend API
│   ├── include/               # Header files (Item.h, Database.h, ApiServer.h, etc.)
│   ├── src/                   # Implementation files
│   ├── SampleDataset/         # CSV dataset (6,678+ price records)
│   ├── CMakeLists.txt         # CMake build configuration
│   └── build/                 # Build output directory
├── Frontend/                  # JavaScript Frontend
│   ├── index.html             # Main HTML file
│   ├── styles.css             # Styles and animations
│   └── app.js                 # Application logic
├── DOCUMENTATION.md           # Complete technical documentation
├── CITATIONS.md               # External resources and attributions
└── README.md                  # This file (competition submission)
```

---

**Built with ❤️ by Team Budgeteer | YEC 2025**
