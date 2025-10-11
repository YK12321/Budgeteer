#ifndef STORE_API_CLIENT_H
#define STORE_API_CLIENT_H

#include "Item.h"
#include <string>
#include <vector>
#include <memory>
#include <map>

/**
 * @class StoreApiClient
 * @brief Handles real-time API integration with retail stores
 */
class StoreApiClient {
private:
    std::string apiKey;
    std::map<std::string, std::string> storeEndpoints;
    
    // API helpers
    std::string makeHttpRequest(const std::string& url, const std::string& method = "GET");
    std::vector<Item> parseWalmartResponse(const std::string& response);
    std::vector<Item> parseLoblawsResponse(const std::string& response);
    std::vector<Item> parseCostcoResponse(const std::string& response);
    
public:
    // Constructor
    StoreApiClient();
    
    // Store API methods
    std::vector<Item> searchWalmart(const std::string& query);
    std::vector<Item> searchLoblaws(const std::string& query);
    std::vector<Item> searchCostco(const std::string& query);
    std::vector<Item> searchAllStores(const std::string& query);
    
    // Price comparison
    std::vector<Item> comparePrices(const std::string& productName);
    
    // Configuration
    void setApiKey(const std::string& key);
    void addStoreEndpoint(const std::string& store, const std::string& endpoint);
};

#endif // STORE_API_CLIENT_H
