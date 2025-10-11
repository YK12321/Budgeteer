#ifndef APISERVER_H
#define APISERVER_H

#include "Database.h"
#include "StoreApiClient.h"
#include "LLMInterface.h"
#include <string>
#include <memory>
#include <map>

/**
 * @class ApiServer
 * @brief Handles API requests and routes with HTTP server support
 */
class ApiServer {
private:
    std::unique_ptr<Database> database;
    std::shared_ptr<StoreApiClient> storeClient;
    std::unique_ptr<LLMInterface> llmInterface;
    int port;
    bool useRealTimeApis;
    
    // Response helpers
    std::string createJsonResponse(const std::vector<Item>& items) const;
    std::string createErrorResponse(const std::string& message) const;
    std::string createStatsResponse(int itemId) const;
    std::string createStoresResponse() const;
    std::string createCategoriesResponse() const;
    std::string createShoppingListResponse(const std::vector<Item>& items) const;
    
    // Request handlers - Database
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
    
    // Request handlers - Real-time APIs
    std::string handleSearchRealTime(const std::string& query);
    std::string handleComparePrices(const std::string& productName);
    
    // Request handlers - LLM Interface
    std::string handleNaturalLanguageQuery(const std::string& query);
    std::string handleGenerateShoppingList(const std::string& request);
    std::string handleBudgetInsight(const std::vector<Item>& items);
    
public:
    // Constructor
    ApiServer(const std::string& dbPath, int serverPort = 8080, bool useRealTime = false);
    
    // Server operations
    bool initialize();
    void printMenu() const;
    void processRequest(int option);
    void run();
    void startHttpServer(); // HTTP server mode
    
    // Configuration
    void setUseRealTimeApis(bool use);
    void setStoreApiKey(const std::string& key);
    
    // Getters
    int getPort() const;
    bool isUsingRealTimeApis() const;
};

#endif // APISERVER_H
