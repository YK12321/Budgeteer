#include "ApiServer.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <httplib.h>
#include <nlohmann/json.hpp>

// Constructor
ApiServer::ApiServer(const std::string& dbPath, int serverPort, bool useRealTime)
    : database(std::make_unique<Database>(dbPath)),
      port(serverPort),
      useRealTimeApis(useRealTime) {
    // Create shared pointer to database for StoreApiClient
    auto dbPtr = std::shared_ptr<Database>(database.get(), [](Database*){});
    storeClient = std::make_shared<StoreApiClient>(dbPtr);
    llmInterface = std::make_unique<LLMInterface>(storeClient);
}

// Initialize the server
bool ApiServer::initialize() {
    std::cout << "Initializing API Server on port " << port << "..." << std::endl;
    std::cout << "Data Source: Sample Dataset (real-time store APIs disabled)" << std::endl;
    
    if (!database->loadFromCSV()) {
        std::cerr << "Failed to load database!" << std::endl;
        return false;
    }
    
    std::cout << "API Server initialized successfully!" << std::endl;
    std::cout << "Loaded " << database->getItemCount() << " items from dataset." << std::endl;
    std::cout << "Store search client ready (using database)." << std::endl;
    std::cout << "LLM interface initialized for natural language processing." << std::endl;
    
    return true;
}

// Create JSON response from items vector
std::string ApiServer::createJsonResponse(const std::vector<Item>& items) const {
    std::ostringstream json;
    json << "{\n";
    json << "  \"success\": true,\n";
    json << "  \"count\": " << items.size() << ",\n";
    json << "  \"items\": [\n";
    
    for (size_t i = 0; i < items.size(); i++) {
        json << "    " << items[i].toJson();
        if (i < items.size() - 1) json << ",";
        json << "\n";
    }
    
    json << "  ]\n";
    json << "}";
    return json.str();
}

// Create error response
std::string ApiServer::createErrorResponse(const std::string& message) const {
    std::ostringstream json;
    json << "{\n";
    json << "  \"success\": false,\n";
    json << "  \"error\": \"" << message << "\"\n";
    json << "}";
    return json.str();
}

// Create statistics response
std::string ApiServer::createStatsResponse(int itemId) const {
    std::ostringstream json;
    json << std::fixed << std::setprecision(2);
    json << "{\n";
    json << "  \"success\": true,\n";
    json << "  \"item_id\": " << itemId << ",\n";
    json << "  \"statistics\": {\n";
    json << "    \"average_price\": " << database->getAveragePrice(itemId) << ",\n";
    json << "    \"min_price\": " << database->getMinPrice(itemId) << ",\n";
    json << "    \"max_price\": " << database->getMaxPrice(itemId) << "\n";
    json << "  }\n";
    json << "}";
    return json.str();
}

// Create stores response
std::string ApiServer::createStoresResponse() const {
    auto stores = database->getAllStores();
    std::ostringstream json;
    json << "{\n";
    json << "  \"success\": true,\n";
    json << "  \"stores\": [";
    
    for (size_t i = 0; i < stores.size(); i++) {
        json << "\"" << stores[i] << "\"";
        if (i < stores.size() - 1) json << ", ";
    }
    
    json << "]\n";
    json << "}";
    return json.str();
}

// Create categories response
std::string ApiServer::createCategoriesResponse() const {
    auto categories = database->getAllCategories();
    std::ostringstream json;
    json << "{\n";
    json << "  \"success\": true,\n";
    json << "  \"categories\": [";
    
    for (size_t i = 0; i < categories.size(); i++) {
        json << "\"" << categories[i] << "\"";
        if (i < categories.size() - 1) json << ", ";
    }
    
    json << "]\n";
    json << "}";
    return json.str();
}

// Create shopping list response
std::string ApiServer::createShoppingListResponse(const std::vector<Item>& items) const {
    std::ostringstream json;
    json << std::fixed << std::setprecision(2);
    json << "{\n";
    json << "  \"success\": true,\n";
    json << "  \"shopping_list\": {\n";
    json << "    \"item_count\": " << items.size() << ",\n";
    json << "    \"total_cost\": ";
    
    double totalCost = 0.0;
    for (const auto& item : items) {
        totalCost += item.getCurrentPrice();
    }
    json << totalCost << ",\n";
    
    json << "    \"items\": [\n";
    for (size_t i = 0; i < items.size(); i++) {
        json << "      " << items[i].toJson();
        if (i < items.size() - 1) json << ",";
        json << "\n";
    }
    json << "    ]\n";
    json << "  }\n";
    json << "}";
    return json.str();
}

// Request handlers
std::string ApiServer::handleGetAllItems() const {
    auto items = database->getAllItems();
    return createJsonResponse(items);
}

std::string ApiServer::handleGetItemById(int itemId) const {
    auto items = database->getItemById(itemId);
    if (items.empty()) {
        return createErrorResponse("Item not found");
    }
    return createJsonResponse(items);
}

std::string ApiServer::handleGetItemsByName(const std::string& name) const {
    auto items = database->getItemsByName(name);
    return createJsonResponse(items);
}

std::string ApiServer::handleGetItemsByStore(const std::string& store) const {
    auto items = database->getItemsByStore(store);
    return createJsonResponse(items);
}

std::string ApiServer::handleGetItemsByCategory(const std::string& category) const {
    auto items = database->getItemsByCategory(category);
    return createJsonResponse(items);
}

std::string ApiServer::handleGetItemsByPriceRange(double minPrice, double maxPrice) const {
    auto items = database->getItemsByPriceRange(minPrice, maxPrice);
    return createJsonResponse(items);
}

std::string ApiServer::handleSearchItems(const std::string& searchTerm) const {
    auto items = database->searchItems(searchTerm);
    return createJsonResponse(items);
}

std::string ApiServer::handleGetStats(int itemId) const {
    auto items = database->getItemById(itemId);
    if (items.empty()) {
        return createErrorResponse("Item not found");
    }
    return createStatsResponse(itemId);
}

std::string ApiServer::handleGetStores() const {
    return createStoresResponse();
}

std::string ApiServer::handleGetCategories() const {
    return createCategoriesResponse();
}

// Store search handlers (using database)
std::string ApiServer::handleSearchRealTime(const std::string& query) {
    std::cout << "[API] Store search (database): " << query << std::endl;
    auto items = storeClient->searchAllStores(query);
    return createJsonResponse(items);
}

std::string ApiServer::handleComparePrices(const std::string& productName) {
    std::cout << "[API] Price comparison (database): " << productName << std::endl;
    auto items = storeClient->comparePrices(productName);
    return createJsonResponse(items);
}

// Helper function to escape special characters for JSON strings
std::string escapeJsonString(const std::string& input) {
    std::ostringstream escaped;
    for (char c : input) {
        switch (c) {
            case '"':  escaped << "\\\""; break;
            case '\\': escaped << "\\\\"; break;
            case '\b': escaped << "\\b"; break;
            case '\f': escaped << "\\f"; break;
            case '\n': escaped << "\\n"; break;
            case '\r': escaped << "\\r"; break;
            case '\t': escaped << "\\t"; break;
            default:
                if (c < 0x20) {
                    // Control characters - use unicode escape
                    escaped << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(c);
                } else {
                    escaped << c;
                }
                break;
        }
    }
    return escaped.str();
}

// LLM Interface handlers
std::string ApiServer::handleNaturalLanguageQuery(const std::string& query) {
    std::cout << "[API] Natural language query: " << query << std::endl;
    
    std::string response = llmInterface->processNaturalLanguageQuery(query);
    
    // Escape both query and response for safe JSON embedding
    std::string escapedQuery = escapeJsonString(query);
    std::string escapedResponse = escapeJsonString(response);
    
    std::ostringstream json;
    json << "{\n";
    json << "  \"success\": true,\n";
    json << "  \"query\": \"" << escapedQuery << "\",\n";
    json << "  \"response\": \"" << escapedResponse << "\"\n";
    json << "}";
    return json.str();
}

std::string ApiServer::handleGenerateShoppingList(const std::string& request) {
    std::cout << "[API] Generate shopping list: " << request << std::endl;
    
    auto items = llmInterface->generateShoppingList(request);
    return createShoppingListResponse(items);
}

std::string ApiServer::handleBudgetInsight(const std::vector<Item>& items) {
    std::cout << "[API] Budget insight for " << items.size() << " items" << std::endl;
    
    std::string insight = llmInterface->getBudgetInsight(items);
    
    std::ostringstream json;
    json << "{\n";
    json << "  \"success\": true,\n";
    json << "  \"insight\": \"" << insight << "\"\n";
    json << "}";
    return json.str();
}

// Print menu
void ApiServer::printMenu() const {
    std::cout << "\n========================================\n";
    std::cout << "      Budgeteer API Menu\n";
    std::cout << "========================================\n";
    std::cout << "DATABASE QUERIES:\n";
    std::cout << "1.  Get all items\n";
    std::cout << "2.  Get item by ID\n";
    std::cout << "3.  Get items by name\n";
    std::cout << "4.  Get items by store\n";
    std::cout << "5.  Get items by category\n";
    std::cout << "6.  Get items by price range\n";
    std::cout << "7.  Search items\n";
    std::cout << "8.  Get item statistics\n";
    std::cout << "9.  Get all stores\n";
    std::cout << "10. Get all categories\n";
    
    if (useRealTimeApis) {
        std::cout << "\nREAL-TIME API QUERIES:\n";
        std::cout << "11. Search real-time (all stores)\n";
        std::cout << "12. Compare prices (real-time)\n";
    }
    
    std::cout << "\nLLM FEATURES:\n";
    std::cout << "13. Natural language query\n";
    std::cout << "14. Generate shopping list (AI)\n";
    std::cout << "15. Get budget insight\n";
    
    std::cout << "\n0.  Exit\n";
    std::cout << "========================================\n";
    std::cout << "Mode: " << (useRealTimeApis ? "Real-time APIs" : "Sample Dataset") << "\n";
    std::cout << "========================================\n";
    std::cout << "Enter option: ";
}

// Process request
void ApiServer::processRequest(int option) {
    std::string response;
    
    switch (option) {
        case 1: {
            std::cout << "\n[API] GET /items\n";
            response = handleGetAllItems();
            break;
        }
        case 2: {
            int itemId;
            std::cout << "Enter item ID: ";
            std::cin >> itemId;
            std::cout << "\n[API] GET /items/" << itemId << "\n";
            response = handleGetItemById(itemId);
            break;
        }
        case 3: {
            std::string name;
            std::cout << "Enter item name: ";
            std::cin.ignore();
            std::getline(std::cin, name);
            std::cout << "\n[API] GET /items?name=" << name << "\n";
            response = handleGetItemsByName(name);
            break;
        }
        case 4: {
            std::string store;
            std::cout << "Enter store name: ";
            std::cin.ignore();
            std::getline(std::cin, store);
            std::cout << "\n[API] GET /items?store=" << store << "\n";
            response = handleGetItemsByStore(store);
            break;
        }
        case 5: {
            std::string category;
            std::cout << "Enter category: ";
            std::cin.ignore();
            std::getline(std::cin, category);
            std::cout << "\n[API] GET /items?category=" << category << "\n";
            response = handleGetItemsByCategory(category);
            break;
        }
        case 6: {
            double minPrice, maxPrice;
            std::cout << "Enter minimum price: ";
            std::cin >> minPrice;
            std::cout << "Enter maximum price: ";
            std::cin >> maxPrice;
            std::cout << "\n[API] GET /items?min=" << minPrice << "&max=" << maxPrice << "\n";
            response = handleGetItemsByPriceRange(minPrice, maxPrice);
            break;
        }
        case 7: {
            std::string searchTerm;
            std::cout << "Enter search term: ";
            std::cin.ignore();
            std::getline(std::cin, searchTerm);
            std::cout << "\n[API] GET /search?q=" << searchTerm << "\n";
            response = handleSearchItems(searchTerm);
            break;
        }
        case 8: {
            int itemId;
            std::cout << "Enter item ID: ";
            std::cin >> itemId;
            std::cout << "\n[API] GET /items/" << itemId << "/stats\n";
            response = handleGetStats(itemId);
            break;
        }
        case 9: {
            std::cout << "\n[API] GET /stores\n";
            response = handleGetStores();
            break;
        }
        case 10: {
            std::cout << "\n[API] GET /categories\n";
            response = handleGetCategories();
            break;
        }
        case 11: {
            if (!useRealTimeApis) {
                response = createErrorResponse("Real-time APIs not enabled");
                break;
            }
            std::string query;
            std::cout << "Enter search query: ";
            std::cin.ignore();
            std::getline(std::cin, query);
            std::cout << "\n[API] GET /api/realtime/search?q=" << query << "\n";
            response = handleSearchRealTime(query);
            break;
        }
        case 12: {
            if (!useRealTimeApis) {
                response = createErrorResponse("Real-time APIs not enabled");
                break;
            }
            std::string product;
            std::cout << "Enter product name: ";
            std::cin.ignore();
            std::getline(std::cin, product);
            std::cout << "\n[API] GET /api/realtime/compare?product=" << product << "\n";
            response = handleComparePrices(product);
            break;
        }
        case 13: {
            std::string query;
            std::cout << "Enter natural language query: ";
            std::cin.ignore();
            std::getline(std::cin, query);
            std::cout << "\n[API] POST /api/llm/query\n";
            response = handleNaturalLanguageQuery(query);
            break;
        }
        case 14: {
            std::string request;
            std::cout << "Describe what you need (e.g., 'snacks under $10'): ";
            std::cin.ignore();
            std::getline(std::cin, request);
            std::cout << "\n[API] POST /api/llm/shopping-list\n";
            response = handleGenerateShoppingList(request);
            break;
        }
        case 15: {
            // Use last search results for budget insight
            auto items = database->getAllItems();
            std::cout << "\n[API] GET /api/llm/budget-insight\n";
            response = handleBudgetInsight(items);
            break;
        }
        case 0: {
            std::cout << "Shutting down server...\n";
            return;
        }
        default: {
            response = createErrorResponse("Invalid option");
            break;
        }
    }
    
    std::cout << "\n[Response]\n" << response << "\n";
}

// Run the server
void ApiServer::run() {
    int option;
    do {
        printMenu();
        std::cin >> option;
        
        if (option == 0) {
            std::cout << "Goodbye!\n";
            break;
        }
        
        processRequest(option);
        
    } while (true);
}

// HTTP Server
void ApiServer::startHttpServer() {
    std::cout << "\n========================================\n";
    std::cout << "  Starting HTTP Server on port " << port << "\n";
    std::cout << "========================================\n";
    
    #ifdef CPPHTTPLIB_HTTPLIB_H
    // cpp-httplib is available
    httplib::Server svr;
    
    // Enable CORS for frontend
    svr.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET, POST, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type"}
    });
    
    // Handle OPTIONS preflight requests
    // Note: CORS headers are already set by set_default_headers, so we just return 200
    svr.Options("/(.*)", [](const httplib::Request&, httplib::Response& res) {
        res.status = 200;
    });
    
    // Root endpoint
    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("{\"message\":\"Budgeteer API Server\",\"version\":\"1.0\",\"status\":\"running\"}", "application/json");
    });
    
    // GET /items - Get all items
    svr.Get("/items", [this](const httplib::Request&, httplib::Response& res) {
        std::cout << "[HTTP] GET /items" << std::endl;
        std::string response = handleGetAllItems();
        res.set_content(response, "application/json");
    });
    
    // GET /items/:id - Get item by ID
    svr.Get("/items/(\\d+)", [this](const httplib::Request& req, httplib::Response& res) {
        int itemId = std::stoi(req.matches[1]);
        std::cout << "[HTTP] GET /items/" << itemId << std::endl;
        std::string response = handleGetItemById(itemId);
        res.set_content(response, "application/json");
    });
    
    // GET /search - Search items
    svr.Get("/search", [this](const httplib::Request& req, httplib::Response& res) {
        if (req.has_param("q")) {
            std::string query = req.get_param_value("q");
            std::cout << "[HTTP] GET /search?q=" << query << std::endl;
            std::string response = handleSearchItems(query);
            res.set_content(response, "application/json");
        } else if (req.has_param("name")) {
            std::string name = req.get_param_value("name");
            std::cout << "[HTTP] GET /search?name=" << name << std::endl;
            std::string response = handleGetItemsByName(name);
            res.set_content(response, "application/json");
        } else if (req.has_param("store")) {
            std::string store = req.get_param_value("store");
            std::cout << "[HTTP] GET /search?store=" << store << std::endl;
            std::string response = handleGetItemsByStore(store);
            res.set_content(response, "application/json");
        } else if (req.has_param("category")) {
            std::string category = req.get_param_value("category");
            std::cout << "[HTTP] GET /search?category=" << category << std::endl;
            std::string response = handleGetItemsByCategory(category);
            res.set_content(response, "application/json");
        } else if (req.has_param("min") && req.has_param("max")) {
            double minPrice = std::stod(req.get_param_value("min"));
            double maxPrice = std::stod(req.get_param_value("max"));
            std::cout << "[HTTP] GET /search?min=" << minPrice << "&max=" << maxPrice << std::endl;
            std::string response = handleGetItemsByPriceRange(minPrice, maxPrice);
            res.set_content(response, "application/json");
        } else {
            res.set_content(createErrorResponse("Missing query parameter"), "application/json");
        }
    });
    
    // GET /stores - Get all stores
    svr.Get("/stores", [this](const httplib::Request&, httplib::Response& res) {
        std::cout << "[HTTP] GET /stores" << std::endl;
        std::string response = handleGetStores();
        res.set_content(response, "application/json");
    });
    
    // GET /categories - Get all categories
    svr.Get("/categories", [this](const httplib::Request&, httplib::Response& res) {
        std::cout << "[HTTP] GET /categories" << std::endl;
        std::string response = handleGetCategories();
        res.set_content(response, "application/json");
    });
    
    // GET /items/:id/stats - Get item statistics
    svr.Get("/items/(\\d+)/stats", [this](const httplib::Request& req, httplib::Response& res) {
        int itemId = std::stoi(req.matches[1]);
        std::cout << "[HTTP] GET /items/" << itemId << "/stats" << std::endl;
        std::string response = handleGetStats(itemId);
        res.set_content(response, "application/json");
    });
    
    // POST /api/llm/query - Natural language query
    svr.Post("/api/llm/query", [this](const httplib::Request& req, httplib::Response& res) {
        std::cout << "[HTTP] POST /api/llm/query" << std::endl;
        try {
            auto json = nlohmann::json::parse(req.body);
            std::string query = json["query"];
            std::string response = handleNaturalLanguageQuery(query);
            res.set_content(response, "application/json");
        } catch (const std::exception& e) {
            res.set_content(createErrorResponse("Invalid JSON body"), "application/json");
        }
    });
    
    // POST /api/llm/shopping-list - Generate shopping list
    svr.Post("/api/llm/shopping-list", [this](const httplib::Request& req, httplib::Response& res) {
        std::cout << "[HTTP] POST /api/llm/shopping-list" << std::endl;
        try {
            auto json = nlohmann::json::parse(req.body);
            std::string prompt = json["prompt"];
            std::string response = handleGenerateShoppingList(prompt);
            res.set_content(response, "application/json");
        } catch (const std::exception& e) {
            res.set_content(createErrorResponse("Invalid JSON body"), "application/json");
        }
    });
    
    // POST /api/llm/budget-insight - Get budget insight
    svr.Post("/api/llm/budget-insight", [this](const httplib::Request& req, httplib::Response& res) {
        std::cout << "[HTTP] POST /api/llm/budget-insight" << std::endl;
        try {
            auto json = nlohmann::json::parse(req.body);
            std::vector<Item> items;
            for (const auto& itemJson : json["items"]) {
                // Parse items from request (simplified)
                auto dbItems = database->getItemById(itemJson["item_id"]);
                items.insert(items.end(), dbItems.begin(), dbItems.end());
            }
            std::string response = handleBudgetInsight(items);
            res.set_content(response, "application/json");
        } catch (const std::exception& e) {
            res.set_content(createErrorResponse("Invalid JSON body"), "application/json");
        }
    });
    
    // GET /api/realtime/search - Real-time search (database fallback)
    svr.Get("/api/realtime/search", [this](const httplib::Request& req, httplib::Response& res) {
        if (req.has_param("q")) {
            std::string query = req.get_param_value("q");
            std::cout << "[HTTP] GET /api/realtime/search?q=" << query << std::endl;
            std::string response = handleSearchRealTime(query);
            res.set_content(response, "application/json");
        } else {
            res.set_content(createErrorResponse("Missing query parameter 'q'"), "application/json");
        }
    });
    
    // GET /api/realtime/compare - Compare prices (database)
    svr.Get("/api/realtime/compare", [this](const httplib::Request& req, httplib::Response& res) {
        if (req.has_param("product")) {
            std::string product = req.get_param_value("product");
            std::cout << "[HTTP] GET /api/realtime/compare?product=" << product << std::endl;
            std::string response = handleComparePrices(product);
            res.set_content(response, "application/json");
        } else {
            res.set_content(createErrorResponse("Missing query parameter 'product'"), "application/json");
        }
    });
    
    std::cout << "\n✓ HTTP Server configured with endpoints" << std::endl;
    std::cout << "✓ CORS enabled for frontend access" << std::endl;
    std::cout << "✓ Ready to accept requests at http://localhost:" << port << std::endl;
    std::cout << "\nAvailable endpoints:" << std::endl;
    std::cout << "  GET  /items" << std::endl;
    std::cout << "  GET  /items/:id" << std::endl;
    std::cout << "  GET  /search?q=..." << std::endl;
    std::cout << "  GET  /stores" << std::endl;
    std::cout << "  GET  /categories" << std::endl;
    std::cout << "  POST /api/llm/query" << std::endl;
    std::cout << "  POST /api/llm/shopping-list" << std::endl;
    std::cout << "\nPress Ctrl+C to stop the server\n" << std::endl;
    
    // Start server
    if (!svr.listen("0.0.0.0", port)) {
        std::cerr << "Failed to start HTTP server on port " << port << std::endl;
        std::cerr << "Port may already be in use." << std::endl;
    }
    
    #else
    std::cout << "\nERROR: cpp-httplib not found!\n";
    std::cout << "HTTP server requires cpp-httplib library.\n";
    std::cout << "Falling back to CLI mode.\n\n";
    run();
    #endif
}

// Configuration
void ApiServer::setUseRealTimeApis(bool use) {
    useRealTimeApis = use;
    std::cout << "[Config] Real-time APIs: " << (use ? "ENABLED" : "DISABLED") << std::endl;
}

void ApiServer::setStoreApiKey(const std::string& key) {
    // Store API keys are no longer used (database-only mode)
    std::cout << "[Config] Store API keys are not needed (using database only)" << std::endl;
}

// Getters
int ApiServer::getPort() const {
    return port;
}

bool ApiServer::isUsingRealTimeApis() const {
    return useRealTimeApis;
}
