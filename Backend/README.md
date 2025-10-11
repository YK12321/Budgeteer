# Budgeteer Backend API

A C++ object-oriented API server for querying items and prices from a database.

## Features

- **Object-Oriented Design**: Clean separation of concerns with Item, Database, and ApiServer classes
- **CSV Database Support**: Load and query data from CSV files
- **Multiple Query Types**:
  - Get all items
  - Search by item ID
  - Search by item name
  - Filter by store
  - Filter by category
  - Filter by price range
  - Full-text search
  - Price statistics (min, max, average)
- **JSON Response Format**: All responses are in JSON format
- **Interactive CLI**: Easy-to-use command-line interface for testing

## Project Structure

```
Backend/
├── include/
│   ├── Item.h           # Item model class
│   ├── Database.h       # Database operations class
│   └── ApiServer.h      # API server class
├── src/
│   ├── Item.cpp         # Item implementation
│   ├── Database.cpp     # Database implementation
│   ├── ApiServer.cpp    # API server implementation
│   └── main.cpp         # Entry point
├── SampleDataset/
│   └── yec_competition_dataset.csv  # Sample data
├── CMakeLists.txt       # CMake build configuration
├── Makefile             # Make build configuration
└── README.md            # This file
```

## Building the Project

### Option 1: Using Make (Recommended for Windows)

```powershell
# Build the project
make

# Run the application
make run

# Clean build files
make clean

# Rebuild from scratch
make rebuild
```

### Option 2: Using CMake

```powershell
# Create build directory
mkdir build
cd build

# Generate build files
cmake ..

# Build the project
cmake --build .

# Run the application
.\bin\BudgeteerAPI.exe
```

## Requirements

- C++17 compatible compiler (g++, MSVC, clang++)
- Make (for Makefile build) or CMake 3.10+ (for CMake build)

## API Endpoints (Simulated)

The current implementation provides a CLI interface that simulates the following REST API endpoints:

- `GET /items` - Get all items
- `GET /items/:id` - Get item by ID
- `GET /items?name=:name` - Search items by name
- `GET /items?store=:store` - Filter by store
- `GET /items?category=:category` - Filter by category
- `GET /items?min=:min&max=:max` - Filter by price range
- `GET /search?q=:query` - Full-text search
- `GET /items/:id/stats` - Get price statistics for an item
- `GET /stores` - Get list of all stores
- `GET /categories` - Get list of all categories

## Example Usage

1. Build and run the application:
   ```powershell
   make run
   ```

2. Select an option from the menu (e.g., option 2 for "Get item by ID")

3. Enter the required parameters (e.g., item ID: 1001)

4. View the JSON response

## Sample Response

```json
{
  "success": true,
  "count": 42,
  "items": [
    {
      "item_id": 1001,
      "item_name": "Samsung 55-inch 4K Smart TV",
      "item_description": "55-inch Crystal UHD Smart TV with HDR",
      "current_price": 705.18,
      "store": "Walmart",
      "category_tags": ["electronics", "entertainment", "home"],
      "image_url": "https://example.com/samsung-55-inch-4k-smart-tv.jpg",
      "price_date": "2022-01-15"
    }
  ]
}
```

## Next Steps

To convert this into a real HTTP REST API server, you can:

1. **Use a C++ HTTP library** such as:
   - [cpp-httplib](https://github.com/yhirose/cpp-httplib) (header-only, simple)
   - [Crow](https://github.com/CrowCpp/Crow) (Flask-like, modern)
   - [Pistache](https://github.com/pistacheio/pistache) (high-performance)
   - [Drogon](https://github.com/drogonframework/drogon) (full-featured)

2. **Add database support** such as:
   - SQLite (embedded)
   - PostgreSQL
   - MySQL

3. **Implement external API integration** for real-time price data

4. **Add authentication** and security features

5. **Deploy** to a cloud platform (AWS, Azure, GCP)

## License

MIT License - Feel free to use this project for learning or commercial purposes.
