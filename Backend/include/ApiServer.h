#ifndef APISERVER_H
#define APISERVER_H

#include "Database.h"
#include <string>
#include <memory>
#include <map>

/**
 * @class ApiServer
 * @brief Handles API requests and routes
 */
class ApiServer {
private:
    std::unique_ptr<Database> database;
    int port;
    
    // Response helpers
    std::string createJsonResponse(const std::vector<Item>& items) const;
    std::string createErrorResponse(const std::string& message) const;
    std::string createStatsResponse(int itemId) const;
    std::string createStoresResponse() const;
    std::string createCategoriesResponse() const;
    
    // Request handlers
    std::string handleGetAllItems() const;
    std::string handleGetItemById(int itemId) const;
    std::string handleGetItemsByName(const std::string& name) const;
    std::string handleGetItemsByStore(const std::string& store) const;
    std::string handleGetItemsByCategory(const std::string& category) const;
    std::string handleGetItemsByPriceRange(double minPrice, double maxPrice) const;
    std::string handleSearchItems(const std::string& searchTerm) const;
    std::string handleGetStats(int itemId) const;
    std::string handleGetStores() const;
    std::string handleGetCategories() const;
    
public:
    // Constructor
    ApiServer(const std::string& dbPath, int serverPort = 8080);
    
    // Server operations
    bool initialize();
    void printMenu() const;
    void processRequest(int option);
    void run();
    
    // Getters
    int getPort() const;
};

#endif // APISERVER_H
