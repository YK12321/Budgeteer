#include "ApiServer.h"
#include <iostream>
#include <sstream>
#include <iomanip>

// Constructor
ApiServer::ApiServer(const std::string& dbPath, int serverPort)
    : database(std::make_unique<Database>(dbPath)), port(serverPort) {}

// Initialize the server
bool ApiServer::initialize() {
    std::cout << "Initializing API Server on port " << port << "..." << std::endl;
    
    if (!database->loadFromCSV()) {
        std::cerr << "Failed to load database!" << std::endl;
        return false;
    }
    
    std::cout << "API Server initialized successfully!" << std::endl;
    std::cout << "Loaded " << database->getItemCount() << " items." << std::endl;
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

// Print menu
void ApiServer::printMenu() const {
    std::cout << "\n========================================\n";
    std::cout << "      Budgeteer API Menu\n";
    std::cout << "========================================\n";
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
    std::cout << "0.  Exit\n";
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

// Getter
int ApiServer::getPort() const {
    return port;
}
