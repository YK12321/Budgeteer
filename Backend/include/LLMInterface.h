#ifndef LLM_INTERFACE_H
#define LLM_INTERFACE_H

#include "Item.h"
#include "StoreApiClient.h"
#include <string>
#include <vector>
#include <memory>
#include <map>

/**
 * @class LLMInterface
 * @brief Natural language interface for shopping queries
 * Implements the LLM instructions for intent recognition and query processing
 */
class LLMInterface {
private:
    std::shared_ptr<StoreApiClient> storeClient;
    
    // Category expansion mappings
    std::map<std::string, std::vector<std::string>> categoryExpansions;
    
    // Intent recognition
    std::string detectIntent(const std::string& query);
    bool isSpecificQuery(const std::string& query);
    bool isGenericQuery(const std::string& query);
    
    // Query processing
    std::vector<std::string> expandCategory(const std::string& category);
    std::string normalizeProductName(const std::string& productName);
    
    // Result ranking
    struct RankedResult {
        std::vector<Item> items;
        double totalCost;
        std::string store;
        double score;
    };
    
    std::vector<RankedResult> rankByCheapestMix(const std::vector<Item>& items);
    std::vector<RankedResult> rankBySingleStore(const std::vector<Item>& items);
    
public:
    // Constructor
    explicit LLMInterface(std::shared_ptr<StoreApiClient> client);
    
    // Query processing modes
    enum class Mode {
        CHEAPEST_MIX,      // Select cheapest item per category across all stores
        SINGLE_STORE,      // Minimize total cost at one store
        BUDGET_INSIGHT     // Provide budget analysis
    };
    
    // Main interface methods
    std::string processNaturalLanguageQuery(const std::string& query, Mode mode = Mode::CHEAPEST_MIX);
    std::vector<Item> generateShoppingList(const std::string& request);
    std::string getBudgetInsight(const std::vector<Item>& items);
    
    // Response formatting
    std::string formatResponse(const std::vector<Item>& items, Mode mode);
    std::string formatTableResponse(const std::vector<Item>& items);
    
    // Configuration
    void addCategoryExpansion(const std::string& category, const std::vector<std::string>& products);
};

#endif // LLM_INTERFACE_H
