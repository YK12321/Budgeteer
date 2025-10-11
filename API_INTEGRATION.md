# API Integration Guide for Real Store APIs

## Overview
This guide explains how to integrate real-time APIs from Walmart, Loblaws, and Costco into the Budgeteer backend.

## Current Status
✅ **Framework Complete**: StoreApiClient provides the structure
⚠️ **API Keys Required**: Need credentials from each store
⚠️ **HTTP Client Needed**: Need to install cpp-httplib or similar

## Step-by-Step Integration

### 1. Walmart API Integration

#### Get API Credentials
1. Visit [Walmart Open API](https://developer.walmart.com/)
2. Sign up for an account
3. Create a new application
4. Get your API Key

#### Available Endpoints
- **Search**: `https://api.walmart.com/v1/search?query={query}&apiKey={key}`
- **Item Details**: `https://api.walmart.com/v1/items/{itemId}?apiKey={key}`
- **Product Recommendations**: `https://api.walmart.com/v1/nbp?itemId={itemId}`

#### Example Response Format
```json
{
  "items": [
    {
      "itemId": 12345,
      "name": "Product Name",
      "salePrice": 19.99,
      "msrp": 24.99,
      "thumbnailImage": "http://...",
      "shortDescription": "Description here",
      "categoryPath": "Electronics/TVs",
      "stock": "Available"
    }
  ]
}
```

#### Implementation in StoreApiClient.cpp

```cpp
std::vector<Item> StoreApiClient::parseWalmartResponse(const std::string& response) {
    std::vector<Item> items;
    
    try {
        nlohmann::json j = nlohmann::json::parse(response);
        
        for (auto& product : j["items"]) {
            Item item;
            
            // Extract basic info
            item.setItemId(product["itemId"].get<int>());
            item.setItemName(product["name"].get<std::string>());
            item.setItemDescription(product.value("shortDescription", ""));
            item.setCurrentPrice(product["salePrice"].get<double>());
            item.setStore("Walmart");
            item.setImageUrl(product.value("thumbnailImage", ""));
            
            // Parse categories
            std::string categoryPath = product.value("categoryPath", "");
            std::vector<std::string> categories;
            // Split categoryPath by '/' and convert to lowercase
            // ... category processing code ...
            
            item.setCategoryTags(categories);
            
            // Set current date
            auto now = std::time(nullptr);
            item.setPriceDate(std::to_string(now));
            
            items.push_back(item);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error parsing Walmart response: " << e.what() << std::endl;
    }
    
    return items;
}
```

### 2. Loblaws/PC Express API

⚠️ **Note**: Loblaws doesn't have a public API. Options:

#### Option A: Web Scraping
Use web scraping with legal compliance:
```cpp
// Use cpp-httplib to fetch HTML pages
// Parse with a library like gumbo-parser or tidy-html5
```

#### Option B: PC Express Unofficial API
Some developers have reverse-engineered endpoints:
- Endpoint pattern: `https://www.loblaws.ca/api/products/search?term={query}`
- May require session handling and headers
- **Risk**: May violate terms of service

#### Option C: Contact Loblaws
- Reach out to Loblaws corporate
- Request API access for your app
- May require partnership agreement

### 3. Costco API

⚠️ **Note**: Costco also doesn't have a public API.

#### Options:
1. **Web Scraping** (with caution)
2. **Unofficial APIs** (may break)
3. **Contact Costco** for partnerships

### 4. Alternative Grocery APIs

#### Instacart API
- Has partnerships with multiple stores
- Provides unified access
- Requires business agreement

#### Other Options:
- **Flipp API**: Flyer data and deals
- **Grocery Gateway**: Ontario-based
- **Amazon Fresh**: If expanding scope

## HTTP Client Implementation

### Install cpp-httplib

#### Method 1: FetchContent (Recommended)
Add to `Backend/CMakeLists.txt`:
```cmake
include(FetchContent)

# cpp-httplib
FetchContent_Declare(
    httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.14.0
)
FetchContent_MakeAvailable(httplib)

# JSON library
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(json)

# Link libraries
target_link_libraries(BudgeteerAPI PRIVATE 
    httplib::httplib
    nlohmann_json::nlohmann_json
)
```

#### Method 2: Manual Installation
1. Download [cpp-httplib](https://github.com/yhirose/cpp-httplib)
2. Download [nlohmann/json](https://github.com/nlohmann/json)
3. Place headers in `Backend/include/external/`

### Update StoreApiClient.cpp

```cpp
#include <httplib.h>
#include <nlohmann/json.hpp>

std::string StoreApiClient::makeHttpRequest(const std::string& url, const std::string& method) {
    try {
        // Parse URL
        size_t schemeEnd = url.find("://");
        size_t hostStart = schemeEnd + 3;
        size_t pathStart = url.find('/', hostStart);
        
        std::string host = url.substr(hostStart, pathStart - hostStart);
        std::string path = url.substr(pathStart);
        
        // Create HTTPS client
        httplib::SSLClient cli(host);
        cli.set_connection_timeout(5, 0); // 5 seconds
        cli.set_read_timeout(10, 0);      // 10 seconds
        
        // Add headers
        httplib::Headers headers = {
            {"User-Agent", "BudgeteerApp/1.0"},
            {"Accept", "application/json"}
        };
        
        // Make request
        auto res = cli.Get(path.c_str(), headers);
        
        if (res && res->status == 200) {
            return res->body;
        } else if (res) {
            std::cerr << "HTTP Error: " << res->status << std::endl;
        } else {
            std::cerr << "Connection failed" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Request error: " << e.what() << std::endl;
    }
    
    return "";
}

std::vector<Item> StoreApiClient::searchWalmart(const std::string& query) {
    std::cout << "[StoreApiClient] Searching Walmart for: " << query << std::endl;
    
    // URL encode query
    std::string encodedQuery = query; // TODO: proper URL encoding
    
    // Build URL
    std::stringstream urlBuilder;
    urlBuilder << storeEndpoints["Walmart"] 
               << "?query=" << encodedQuery;
    
    if (!apiKey.empty()) {
        urlBuilder << "&apiKey=" << apiKey;
    }
    
    std::string url = urlBuilder.str();
    
    // Make request
    std::string response = makeHttpRequest(url);
    
    if (response.empty()) {
        std::cerr << "Empty response from Walmart API" << std::endl;
        return {};
    }
    
    // Parse and return
    return parseWalmartResponse(response);
}
```

## Rate Limiting

Implement rate limiting to respect API quotas:

```cpp
class RateLimiter {
private:
    std::map<std::string, std::deque<std::chrono::steady_clock::time_point>> requestTimes;
    std::map<std::string, int> limits;
    std::chrono::seconds window{60};
    std::mutex mtx;
    
public:
    void setLimit(const std::string& endpoint, int requestsPerMinute) {
        limits[endpoint] = requestsPerMinute;
    }
    
    bool allowRequest(const std::string& endpoint) {
        std::lock_guard<std::mutex> lock(mtx);
        
        auto now = std::chrono::steady_clock::now();
        auto& times = requestTimes[endpoint];
        
        // Remove old requests
        while (!times.empty() && 
               std::chrono::duration_cast<std::chrono::seconds>(now - times.front()) > window) {
            times.pop_front();
        }
        
        // Check limit
        if (times.size() >= limits[endpoint]) {
            return false;
        }
        
        // Add new request
        times.push_back(now);
        return true;
    }
};

// Usage in StoreApiClient
RateLimiter rateLimiter;

// In constructor
rateLimiter.setLimit("Walmart", 5);  // 5 requests per minute
rateLimiter.setLimit("Loblaws", 10);
rateLimiter.setLimit("Costco", 5);

// Before making request
if (!rateLimiter.allowRequest("Walmart")) {
    std::cerr << "Rate limit exceeded for Walmart API" << std::endl;
    return {};
}
```

## Caching Strategy

Implement caching to reduce API calls:

```cpp
class PriceCache {
private:
    struct CachedItem {
        std::vector<Item> items;
        std::chrono::steady_clock::time_point timestamp;
    };
    
    std::map<std::string, CachedItem> cache;
    std::chrono::minutes cacheExpiry{60}; // 60 minutes
    std::mutex mtx;
    
public:
    bool get(const std::string& query, std::vector<Item>& items) {
        std::lock_guard<std::mutex> lock(mtx);
        
        auto it = cache.find(query);
        if (it == cache.end()) {
            return false;
        }
        
        auto now = std::chrono::steady_clock::now();
        auto age = std::chrono::duration_cast<std::chrono::minutes>(
            now - it->second.timestamp
        );
        
        if (age > cacheExpiry) {
            cache.erase(it);
            return false;
        }
        
        items = it->second.items;
        return true;
    }
    
    void put(const std::string& query, const std::vector<Item>& items) {
        std::lock_guard<std::mutex> lock(mtx);
        
        cache[query] = {
            items,
            std::chrono::steady_clock::now()
        };
    }
};

// Usage
PriceCache cache;

std::vector<Item> StoreApiClient::searchWalmart(const std::string& query) {
    // Check cache first
    std::vector<Item> cachedItems;
    if (cache.get("walmart:" + query, cachedItems)) {
        std::cout << "[Cache] Using cached results" << std::endl;
        return cachedItems;
    }
    
    // Make API request...
    std::vector<Item> items = /* ... */;
    
    // Cache results
    cache.put("walmart:" + query, items);
    
    return items;
}
```

## Error Handling

```cpp
enum class ApiError {
    SUCCESS,
    NETWORK_ERROR,
    RATE_LIMIT_EXCEEDED,
    INVALID_API_KEY,
    INVALID_RESPONSE,
    TIMEOUT,
    UNKNOWN
};

struct ApiResult {
    ApiError error;
    std::vector<Item> items;
    std::string message;
};

ApiResult StoreApiClient::searchWalmartSafe(const std::string& query) {
    ApiResult result;
    result.error = ApiError::SUCCESS;
    
    try {
        // Check rate limit
        if (!rateLimiter.allowRequest("Walmart")) {
            result.error = ApiError::RATE_LIMIT_EXCEEDED;
            result.message = "Rate limit exceeded. Please try again later.";
            return result;
        }
        
        // Make request
        std::string response = makeHttpRequest(/* ... */);
        
        if (response.empty()) {
            result.error = ApiError::NETWORK_ERROR;
            result.message = "Failed to connect to Walmart API";
            return result;
        }
        
        // Parse response
        result.items = parseWalmartResponse(response);
        result.message = "Success";
        
    } catch (const std::exception& e) {
        result.error = ApiError::UNKNOWN;
        result.message = std::string("Error: ") + e.what();
    }
    
    return result;
}
```

## Testing

### Unit Tests
```cpp
// test_store_api.cpp
#include <gtest/gtest.h>
#include "StoreApiClient.h"

TEST(StoreApiClient, ParseWalmartResponse) {
    std::string mockResponse = R"({
        "items": [{
            "itemId": 123,
            "name": "Test Product",
            "salePrice": 9.99
        }]
    })";
    
    StoreApiClient client;
    auto items = client.parseWalmartResponse(mockResponse);
    
    ASSERT_EQ(items.size(), 1);
    EXPECT_EQ(items[0].getItemName(), "Test Product");
    EXPECT_DOUBLE_EQ(items[0].getCurrentPrice(), 9.99);
}
```

### Integration Tests
```cpp
TEST(StoreApiClient, RateLimiting) {
    StoreApiClient client;
    
    // Make multiple requests
    for (int i = 0; i < 10; i++) {
        auto result = client.searchWalmart("test");
        // Should succeed for first 5, fail for next 5
    }
}
```

## Deployment Considerations

1. **API Keys Security**:
   - Store in environment variables
   - Never commit to git
   - Use secrets management service

2. **Monitoring**:
   - Log API response times
   - Track error rates
   - Monitor cache hit rates

3. **Fallback Strategy**:
   - Use cached data if API fails
   - Fallback to CSV database
   - Show "prices may be outdated" warning

## Legal Considerations

⚠️ **Important**: 
- Review each store's Terms of Service
- Respect robots.txt for web scraping
- Comply with rate limits
- Attribute data sources properly
- Consider partnership agreements

## Summary

✅ **Ready**: Framework complete
📋 **TODO**: Get API credentials
🔧 **TODO**: Install HTTP client
🧪 **TODO**: Test with real APIs
📚 **TODO**: Add comprehensive error handling

The infrastructure is in place - just add the API keys and HTTP library to go live!
