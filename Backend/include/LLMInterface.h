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
 * Implements GPT-4o-mini integration for intent recognition and query processing
 */
class LLMInterface {
private:
    std::shared_ptr<StoreApiClient> storeClient;
    
    // GPT API configuration
    std::string openaiApiKey;
    bool useGPT;
    std::string gptModel;
    int maxTokens;
    double temperature;
    
    // Usage tracking
    int dailyQueryCount;
    int dailyQueryLimit;
    
    // Category expansion mappings
    std::map<std::string, std::vector<std::string>> categoryExpansions;

public:
    // Query processing modes (declared early for use in private methods)
    enum class Mode {
        CHEAPEST_MIX,      // Select cheapest item per category across all stores
        SINGLE_STORE,      // Minimize total cost at one store
        BUDGET_INSIGHT     // Provide budget analysis
    };

private:
    // GPT API methods
    std::string callGPTAPI(const std::string& prompt);
    std::string buildPrompt(const std::string& query, const std::string& context);
    bool canMakeGPTRequest();
    std::vector<Item> cherryPickRelevantItems(const std::string& query, const std::vector<Item>& items);
    
    // Reasoning methods
    struct ReasoningResult {
        bool isComplete;
        std::string reasoning;
        std::vector<std::string> missingItems;
        std::vector<std::string> unnecessaryItems;
    };
    
    ReasoningResult reasonAboutShoppingList(const std::string& originalQuery, const std::vector<std::string>& currentItems);
    std::vector<Item> refineShoppingListWithReasoning(const std::string& query, std::vector<Item> initialItems, int maxIterations = 3);
    
    // Local fallback methods
    std::string detectIntentLocal(const std::string& query);
    bool isSpecificQuery(const std::string& query);
    bool isGenericQuery(const std::string& query);
    bool isSimpleQuery(const std::string& query);
    
    // Query processing
    std::vector<std::string> expandCategory(const std::string& category);
    std::string normalizeProductName(const std::string& productName);
    
    // Processing methods
    std::string processQueryWithGPT(const std::string& query, Mode mode);
    std::string processQueryLocally(const std::string& query, Mode mode);
    
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
    
    // Configuration methods
    void setOpenAIKey(const std::string& key);
    void enableGPTMode(bool enable);
    void setDailyQueryLimit(int limit);
    void setGPTModel(const std::string& model);
    std::string getGPTModel() const;
    
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
