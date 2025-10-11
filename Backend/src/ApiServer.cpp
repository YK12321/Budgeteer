#include "ApiServer.h"
#include <iostream>
#include <sstream>
#include <iomanip>

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

// LLM Interface handlers
std::string ApiServer::handleNaturalLanguageQuery(const std::string& query) {
    std::cout << "[API] Natural language query: " << query << std::endl;
    
    std::string response = llmInterface->processNaturalLanguageQuery(query);
    
    std::ostringstream json;
    json << "{\n";
    json << "  \"success\": true,\n";
    json << "  \"query\": \"" << query << "\",\n";
    json << "  \"response\": \"" << response << "\"\n";
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
    std::cout << "\nNOTE: HTTP server requires cpp-httplib library.\n";
    std::cout << "To enable HTTP mode:\n";
    std::cout << "1. Install cpp-httplib: https://github.com/yhirose/cpp-httplib\n";
    std::cout << "2. Update CMakeLists.txt to link the library\n";
    std::cout << "3. Implement HTTP endpoints in this method\n\n";
    std::cout << "For now, using CLI mode instead.\n";
    std::cout << "========================================\n\n";
    
    // Fall back to CLI mode
    run();
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
