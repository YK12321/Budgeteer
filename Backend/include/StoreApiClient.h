#ifndef STORE_API_CLIENT_H
#define STORE_API_CLIENT_H

#include "Item.h"
#include "Database.h"
#include <string>
#include <vector>
#include <memory>

/**
 * @class StoreApiClient
 * @brief Handles product searches using the sample database
 * Note: Real-time store APIs have been removed. This class now searches the local database.
 */
class StoreApiClient {
private:
    std::shared_ptr<Database> database;
    
public:
    // Constructor
    explicit StoreApiClient(std::shared_ptr<Database> db);
    
    // Store search methods (now using database)
    std::vector<Item> searchWalmart(const std::string& query);
    std::vector<Item> searchLoblaws(const std::string& query);
    std::vector<Item> searchCostco(const std::string& query);
    std::vector<Item> searchAllStores(const std::string& query);
    
    // Price comparison
    std::vector<Item> comparePrices(const std::string& productName);
};

#endif // STORE_API_CLIENT_H
