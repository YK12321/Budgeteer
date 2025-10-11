#ifndef DATABASE_H
#define DATABASE_H

#include "Item.h"
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

/**
 * @class Database
 * @brief Handles database operations and CSV file parsing
 */
class Database {
private:
    std::vector<Item> items;
    std::string csvFilePath;
    
    // Helper methods
    std::vector<std::string> splitString(const std::string& str, char delimiter) const;
    std::vector<std::string> parseCategories(const std::string& categoriesStr);
    int calculateLevenshteinDistance(const std::string& s1, const std::string& s2) const;
    double calculateSimilarity(const std::string& s1, const std::string& s2) const;
    bool containsWord(const std::string& text, const std::string& word) const;
    
public:
    // Constructor
    explicit Database(const std::string& filePath);
    
    // Database operations
    bool loadFromCSV();
    int getItemCount() const;
    
    // Query methods
    std::vector<Item> getAllItems() const;
    std::vector<Item> getItemById(int itemId) const;
    std::vector<Item> getItemsByName(const std::string& name) const;
    std::vector<Item> getItemsByStore(const std::string& store) const;
    std::vector<Item> getItemsByCategory(const std::string& category) const;
    std::vector<Item> getItemsByPriceRange(double minPrice, double maxPrice) const;
    std::vector<Item> searchItems(const std::string& searchTerm) const;
    
    // Statistics methods
    double getAveragePrice(int itemId) const;
    double getMinPrice(int itemId) const;
    double getMaxPrice(int itemId) const;
    std::vector<std::string> getAllStores() const;
    std::vector<std::string> getAllCategories() const;
};

#endif // DATABASE_H
