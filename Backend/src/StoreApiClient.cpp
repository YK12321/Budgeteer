#include "StoreApiClient.h"
#include <iostream>
#include <algorithm>

// Constructor - accepts database reference
StoreApiClient::StoreApiClient(std::shared_ptr<Database> db) 
    : database(db) {
    std::cout << "[StoreApiClient] Initialized with sample database (real-time APIs disabled)" << std::endl;
}

// Search Walmart items from database
std::vector<Item> StoreApiClient::searchWalmart(const std::string& query) {
    if (!database) {
        std::cerr << "[StoreApiClient] Error: Database not initialized" << std::endl;
        return {};
    }
    
    std::cout << "[StoreApiClient] Searching Walmart database for: " << query << std::endl;
    
    // Get all Walmart items that match the search
    auto allResults = database->searchItems(query);
    std::vector<Item> walmartItems;
    
    for (const auto& item : allResults) {
        if (item.getStore() == "Walmart") {
            walmartItems.push_back(item);
        }
    }
    
    std::cout << "[StoreApiClient] Found " << walmartItems.size() << " Walmart items" << std::endl;
    return walmartItems;
}

// Search Loblaws items from database
std::vector<Item> StoreApiClient::searchLoblaws(const std::string& query) {
    if (!database) {
        std::cerr << "[StoreApiClient] Error: Database not initialized" << std::endl;
        return {};
    }
    
    std::cout << "[StoreApiClient] Searching Loblaws database for: " << query << std::endl;
    
    auto allResults = database->searchItems(query);
    std::vector<Item> loblawsItems;
    
    for (const auto& item : allResults) {
        if (item.getStore() == "Loblaws") {
            loblawsItems.push_back(item);
        }
    }
    
    std::cout << "[StoreApiClient] Found " << loblawsItems.size() << " Loblaws items" << std::endl;
    return loblawsItems;
}

// Search Costco items from database
std::vector<Item> StoreApiClient::searchCostco(const std::string& query) {
    if (!database) {
        std::cerr << "[StoreApiClient] Error: Database not initialized" << std::endl;
        return {};
    }
    
    std::cout << "[StoreApiClient] Searching Costco database for: " << query << std::endl;
    
    auto allResults = database->searchItems(query);
    std::vector<Item> costcoItems;
    
    for (const auto& item : allResults) {
        if (item.getStore() == "Costco") {
            costcoItems.push_back(item);
        }
    }
    
    std::cout << "[StoreApiClient] Found " << costcoItems.size() << " Costco items" << std::endl;
    return costcoItems;
}

// Search all stores from database
std::vector<Item> StoreApiClient::searchAllStores(const std::string& query) {
    if (!database) {
        std::cerr << "[StoreApiClient] Error: Database not initialized" << std::endl;
        return {};
    }
    
    std::cout << "[StoreApiClient] Searching all stores in database for: " << query << std::endl;
    
    // Use database search which already searches across all stores
    auto items = database->searchItems(query);
    
    std::cout << "[StoreApiClient] Found " << items.size() << " total items across all stores" << std::endl;
    return items;
}

// Compare prices across all stores from database
std::vector<Item> StoreApiClient::comparePrices(const std::string& productName) {
    if (!database) {
        std::cerr << "[StoreApiClient] Error: Database not initialized" << std::endl;
        return {};
    }
    
    std::cout << "[StoreApiClient] Comparing prices in database for: " << productName << std::endl;
    
    // Search all stores
    auto items = searchAllStores(productName);
    
    // Sort by price (lowest first)
    std::sort(items.begin(), items.end(), [](const Item& a, const Item& b) {
        return a.getCurrentPrice() < b.getCurrentPrice();
    });
    
    std::cout << "[StoreApiClient] Sorted " << items.size() << " items by price" << std::endl;
    return items;
}
