#include "Database.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <set>

// Constructor
Database::Database(const std::string& filePath) : csvFilePath(filePath) {}

// Helper function to split string by delimiter
std::vector<std::string> Database::splitString(const std::string& str, char delimiter) const {
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
    return static_cast<int>(items.size());
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

// Calculate Levenshtein distance for string similarity
int Database::calculateLevenshteinDistance(const std::string& s1, const std::string& s2) const {
    const size_t len1 = s1.size(), len2 = s2.size();
    std::vector<std::vector<int>> d(len1 + 1, std::vector<int>(len2 + 1));

    d[0][0] = 0;
    for(size_t i = 1; i <= len1; ++i) d[i][0] = i;
    for(size_t i = 1; i <= len2; ++i) d[0][i] = i;

    for(size_t i = 1; i <= len1; ++i) {
        for(size_t j = 1; j <= len2; ++j) {
            d[i][j] = std::min({
                d[i - 1][j] + 1,
                d[i][j - 1] + 1,
                d[i - 1][j - 1] + (s1[i - 1] == s2[j - 1] ? 0 : 1)
            });
        }
    }
    return d[len1][len2];
}

// Calculate similarity score (0.0 to 1.0, higher is more similar)
double Database::calculateSimilarity(const std::string& s1, const std::string& s2) const {
    if (s1.empty() || s2.empty()) return 0.0;
    
    int distance = calculateLevenshteinDistance(s1, s2);
    size_t maxLen = std::max(s1.length(), s2.length());
    
    return 1.0 - (static_cast<double>(distance) / static_cast<double>(maxLen));
}

// Check if text contains word (case-insensitive)
bool Database::containsWord(const std::string& text, const std::string& word) const {
    std::string lowerText = text;
    std::string lowerWord = word;
    std::transform(lowerText.begin(), lowerText.end(), lowerText.begin(), ::tolower);
    std::transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(), ::tolower);
    
    // Check for whole word match or partial match
    return lowerText.find(lowerWord) != std::string::npos;
}

std::vector<Item> Database::searchItems(const std::string& searchTerm) const {
    if (searchTerm.empty()) return {};
    
    std::vector<std::pair<Item, double>> scoredItems;
    std::string lowerSearchTerm = searchTerm;
    std::transform(lowerSearchTerm.begin(), lowerSearchTerm.end(), 
                   lowerSearchTerm.begin(), ::tolower);
    
    // Split search term into words
    std::vector<std::string> searchWords = splitString(searchTerm, ' ');
    
    // OPTIMIZATION: Use a score threshold to filter early
    const double MIN_SCORE_THRESHOLD = 15.0;
    const int MAX_RESULTS = 50;  // Limit results for token efficiency
    
    for (const auto& item : items) {
        double score = 0.0;
        std::string itemName = item.getItemName();
        std::string itemDesc = item.getItemDescription();
        std::string lowerName = itemName;
        std::string lowerDesc = itemDesc;
        
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        std::transform(lowerDesc.begin(), lowerDesc.end(), lowerDesc.begin(), ::tolower);
        
        // OPTIMIZATION 1: Exact/prefix match gets massive boost (helps with "flour", "sugar", etc.)
        if (lowerName == lowerSearchTerm) {
            score += 200.0;  // Perfect match
        } else if (lowerName.find(lowerSearchTerm + " ") == 0 || 
                   lowerName.find(lowerSearchTerm + " (") == 0) {
            score += 150.0;  // Starts with search term (e.g., "Flour (5kg)")
        } else if (lowerName.find(lowerSearchTerm) != std::string::npos) {
            score += 100.0;  // Contains search term
        }
        
        // Exact match in description (lower priority)
        if (lowerDesc.find(lowerSearchTerm) != std::string::npos) {
            score += 40.0;
        }
        
        // OPTIMIZATION 2: Only calculate expensive similarity if we don't have a good match yet
        if (score < 100.0) {
            // Calculate similarity score for the full name
            double nameSimilarity = calculateSimilarity(lowerSearchTerm, lowerName);
            score += nameSimilarity * 60.0;
        }
        
        // Check individual words for partial matches
        for (const auto& word : searchWords) {
            if (word.length() >= 3) {  // Only check words with 3+ characters
                if (containsWord(itemName, word)) {
                    score += 25.0;
                }
                if (containsWord(itemDesc, word)) {
                    score += 10.0;
                }
            }
        }
        
        // OPTIMIZATION 3: Early filtering - only keep items above threshold
        if (score > MIN_SCORE_THRESHOLD) {
            scoredItems.push_back({item, score});
        }
    }
    
    // Sort by score (highest first)
    std::sort(scoredItems.begin(), scoredItems.end(),
              [](const auto& a, const auto& b) {
                  return a.second > b.second;
              });
    
    // OPTIMIZATION 4: Limit results to top MAX_RESULTS for token efficiency
    std::vector<Item> result;
    int count = 0;
    for (const auto& pair : scoredItems) {
        result.push_back(pair.first);
        count++;
        if (count >= MAX_RESULTS) {
            break;
        }
    }
    
    return result;
}

// Statistics methods
double Database::getAveragePrice(int itemId) const {
    auto itemList = getItemById(itemId);
    if (itemList.empty()) return 0.0;
    
    double sum = 0.0;
    for (const auto& item : itemList) {
        sum += item.getCurrentPrice();
    }
    return sum / itemList.size();
}

double Database::getMinPrice(int itemId) const {
    auto itemList = getItemById(itemId);
    if (itemList.empty()) return 0.0;
    
    double minPrice = itemList[0].getCurrentPrice();
    for (const auto& item : itemList) {
        if (item.getCurrentPrice() < minPrice) {
            minPrice = item.getCurrentPrice();
        }
    }
    return minPrice;
}

double Database::getMaxPrice(int itemId) const {
    auto itemList = getItemById(itemId);
    if (itemList.empty()) return 0.0;
    
    double maxPrice = itemList[0].getCurrentPrice();
    for (const auto& item : itemList) {
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
