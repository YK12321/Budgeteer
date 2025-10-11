#include "Database.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <set>

// Constructor
Database::Database(const std::string& filePath) : csvFilePath(filePath) {}

// Helper function to split string by delimiter
std::vector<std::string> Database::splitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    
    while (std::getline(tokenStream, token, delimiter)) {
        // Trim whitespace
        token.erase(0, token.find_first_not_of(" \t\r\n"));
        token.erase(token.find_last_not_of(" \t\r\n") + 1);
        tokens.push_back(token);
    }
    
    return tokens;
}

// Parse category tags from comma-separated string
std::vector<std::string> Database::parseCategories(const std::string& categoriesStr) {
    return splitString(categoriesStr, ',');
}

// Load data from CSV file
bool Database::loadFromCSV() {
    std::ifstream file(csvFilePath);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << csvFilePath << std::endl;
        return false;
    }
    
    std::string line;
    bool isFirstLine = true;
    
    while (std::getline(file, line)) {
        // Skip header line
        if (isFirstLine) {
            isFirstLine = false;
            continue;
        }
        
        // Parse CSV line
        std::vector<std::string> fields;
        std::string field;
        bool inQuotes = false;
        
        for (char c : line) {
            if (c == '"') {
                inQuotes = !inQuotes;
            } else if (c == ',' && !inQuotes) {
                fields.push_back(field);
                field.clear();
            } else {
                field += c;
            }
        }
        fields.push_back(field); // Add last field
        
        // Validate field count
        if (fields.size() != 8) {
            std::cerr << "Warning: Skipping invalid line (expected 8 fields, got " 
                      << fields.size() << ")" << std::endl;
            continue;
        }
        
        try {
            // Create Item object
            int itemId = std::stoi(fields[0]);
            std::string itemName = fields[1];
            std::string itemDescription = fields[2];
            double currentPrice = std::stod(fields[3]);
            std::string store = fields[4];
            std::vector<std::string> categoryTags = parseCategories(fields[5]);
            std::string imageUrl = fields[6];
            std::string priceDate = fields[7];
            
            Item item(itemId, itemName, itemDescription, currentPrice, 
                     store, categoryTags, imageUrl, priceDate);
            items.push_back(item);
            
        } catch (const std::exception& e) {
            std::cerr << "Warning: Error parsing line: " << e.what() << std::endl;
        }
    }
    
    file.close();
    std::cout << "Successfully loaded " << items.size() << " items from database." << std::endl;
    return true;
}

// Get total item count
int Database::getItemCount() const {
    return items.size();
}

// Query methods
std::vector<Item> Database::getAllItems() const {
    return items;
}

std::vector<Item> Database::getItemById(int itemId) const {
    std::vector<Item> result;
    for (const auto& item : items) {
        if (item.getItemId() == itemId) {
            result.push_back(item);
        }
    }
    return result;
}

std::vector<Item> Database::getItemsByName(const std::string& name) const {
    std::vector<Item> result;
    for (const auto& item : items) {
        if (item.getItemName().find(name) != std::string::npos) {
            result.push_back(item);
        }
    }
    return result;
}

std::vector<Item> Database::getItemsByStore(const std::string& store) const {
    std::vector<Item> result;
    for (const auto& item : items) {
        if (item.getStore() == store) {
            result.push_back(item);
        }
    }
    return result;
}

std::vector<Item> Database::getItemsByCategory(const std::string& category) const {
    std::vector<Item> result;
    for (const auto& item : items) {
        if (item.hasCategory(category)) {
            result.push_back(item);
        }
    }
    return result;
}

std::vector<Item> Database::getItemsByPriceRange(double minPrice, double maxPrice) const {
    std::vector<Item> result;
    for (const auto& item : items) {
        double price = item.getCurrentPrice();
        if (price >= minPrice && price <= maxPrice) {
            result.push_back(item);
        }
    }
    return result;
}

std::vector<Item> Database::searchItems(const std::string& searchTerm) const {
    std::vector<Item> result;
    std::string lowerSearchTerm = searchTerm;
    std::transform(lowerSearchTerm.begin(), lowerSearchTerm.end(), 
                   lowerSearchTerm.begin(), ::tolower);
    
    for (const auto& item : items) {
        std::string lowerName = item.getItemName();
        std::string lowerDesc = item.getItemDescription();
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        std::transform(lowerDesc.begin(), lowerDesc.end(), lowerDesc.begin(), ::tolower);
        
        if (lowerName.find(lowerSearchTerm) != std::string::npos ||
            lowerDesc.find(lowerSearchTerm) != std::string::npos) {
            result.push_back(item);
        }
    }
    return result;
}

// Statistics methods
double Database::getAveragePrice(int itemId) const {
    auto items = getItemById(itemId);
    if (items.empty()) return 0.0;
    
    double sum = 0.0;
    for (const auto& item : items) {
        sum += item.getCurrentPrice();
    }
    return sum / items.size();
}

double Database::getMinPrice(int itemId) const {
    auto items = getItemById(itemId);
    if (items.empty()) return 0.0;
    
    double minPrice = items[0].getCurrentPrice();
    for (const auto& item : items) {
        if (item.getCurrentPrice() < minPrice) {
            minPrice = item.getCurrentPrice();
        }
    }
    return minPrice;
}

double Database::getMaxPrice(int itemId) const {
    auto items = getItemById(itemId);
    if (items.empty()) return 0.0;
    
    double maxPrice = items[0].getCurrentPrice();
    for (const auto& item : items) {
        if (item.getCurrentPrice() > maxPrice) {
            maxPrice = item.getCurrentPrice();
        }
    }
    return maxPrice;
}

std::vector<std::string> Database::getAllStores() const {
    std::set<std::string> uniqueStores;
    for (const auto& item : items) {
        uniqueStores.insert(item.getStore());
    }
    return std::vector<std::string>(uniqueStores.begin(), uniqueStores.end());
}

std::vector<std::string> Database::getAllCategories() const {
    std::set<std::string> uniqueCategories;
    for (const auto& item : items) {
        auto tags = item.getCategoryTags();
        uniqueCategories.insert(tags.begin(), tags.end());
    }
    return std::vector<std::string>(uniqueCategories.begin(), uniqueCategories.end());
}
