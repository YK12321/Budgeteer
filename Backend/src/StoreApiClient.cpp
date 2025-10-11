/**
 * @file StoreApiClient.cpp
 * @brief Implementation of StoreApiClient - handles multi-store product search
 * 
 * This class abstracts store-specific search logic and provides a unified
 * interface for searching products across multiple retailers. Originally designed
 * to integrate with real-time store APIs, it now operates on the local database
 * for reliability and cost-effectiveness.
 * 
 * Search Strategy:
 * - Uses Database class for actual search execution
 * - Filters results by store name
 * - Aggregates results across all stores
 * - Sorts by relevance and price
 * 
 * @author York Entrepreneurship Competition Team
 * @date October 2025
 * @version 1.0
 */

#include "StoreApiClient.h"
#include <iostream>
#include <algorithm>

/**
 * @brief Constructor - initializes StoreApiClient with database reference
 * 
 * Creates a StoreApiClient instance that uses the provided Database
 * for executing product searches. The database must remain valid for
 * the lifetime of this StoreApiClient instance.
 * 
 * @param db Shared pointer to initialized Database instance
 * 
 * Design Note: Real-time store APIs were removed in favor of database-only
 * operation for improved reliability and to avoid external API dependencies.
 */
StoreApiClient::StoreApiClient(std::shared_ptr<Database> db) 
    : database(db) {
    std::cout << "[StoreApiClient] Initialized with sample database (real-time APIs disabled)" << std::endl;
}

/**
 * @brief Search for products at Walmart
 * 
 * Searches the database for products matching the query and filters
 * results to include only Walmart items.
 * 
 * @param query Search term (product name, description, or keywords)
 * @return std::vector<Item> Vector of matching Walmart items (empty if no matches)
 * 
 * Process:
 * 1. Validates database is initialized
 * 2. Executes database search with query
 * 3. Filters results to Walmart store
 * 4. Returns filtered results
 * 
 * Example usage:
 *   auto results = client->searchWalmart("milk");
 *   // Returns all milk products available at Walmart
 */
std::vector<Item> StoreApiClient::searchWalmart(const std::string& query) {
    if (!database) {
        std::cerr << "[StoreApiClient] Error: Database not initialized" << std::endl;
        return {};
    }
    
    std::cout << "[StoreApiClient] Searching Walmart database for: " << query << std::endl;
    
    // Execute database search (uses intelligent ranking algorithm)
    auto allResults = database->searchItems(query);
    
    // Filter results to Walmart items only
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

/**
 * @brief Search for products across all stores
 * 
 * Performs a comprehensive search across all available stores
 * (Walmart, Loblaws, Costco) and returns all matching items.
 * 
 * @param query Search term (product name, description, or keywords)
 * @return std::vector<Item> Vector of matching items from all stores
 * 
 * This is the primary search method used by:
 * - Basic search feature in frontend
 * - AI-powered search for product discovery
 * - Shopping list generation
 * 
 * The database search algorithm handles:
 * - Exact matching
 * - Fuzzy matching (typos, similar words)
 * - Partial matching
 * - Relevance ranking
 */
std::vector<Item> StoreApiClient::searchAllStores(const std::string& query) {
    if (!database) {
        std::cerr << "[StoreApiClient] Error: Database not initialized" << std::endl;
        return {};
    }
    
    std::cout << "[StoreApiClient] Searching all stores in database for: " << query << std::endl;
    
    // Use database search which already searches across all stores
    // Results are pre-sorted by relevance score
    auto items = database->searchItems(query);
    
    std::cout << "[StoreApiClient] Found " << items.size() << " total items across all stores" << std::endl;
    return items;
}

/**
 * @brief Compare prices for a product across all stores
 * 
 * Searches for a product across all stores and returns results
 * sorted by price (lowest to highest). This enables users to
 * quickly identify the best deal for a specific product.
 * 
 * @param productName Product name to search and compare
 * @return std::vector<Item> Items sorted by price (ascending)
 * 
 * Use Cases:
 * - Price comparison feature
 * - Finding best deals
 * - Budget optimization
 * - Shopping list cost minimization
 * 
 * Example:
 *   auto prices = client->comparePrices("2% Milk");
 *   // Returns milk from all stores, sorted by price
 *   // User can see which store has the cheapest milk
 */
std::vector<Item> StoreApiClient::comparePrices(const std::string& productName) {
    if (!database) {
        std::cerr << "[StoreApiClient] Error: Database not initialized" << std::endl;
        return {};
    }
    
    std::cout << "[StoreApiClient] Comparing prices in database for: " << productName << std::endl;
    
    // Search across all stores for the product
    auto items = searchAllStores(productName);
    
    // Sort by price in ascending order (cheapest first)
    // Lambda function compares currentPrice of two items
    std::sort(items.begin(), items.end(), [](const Item& a, const Item& b) {
        return a.getCurrentPrice() < b.getCurrentPrice();
    });
    
    std::cout << "[StoreApiClient] Sorted " << items.size() << " items by price" << std::endl;
    return items;
}
