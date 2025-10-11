#include "LLMInterface.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <cctype>

LLMInterface::LLMInterface(std::shared_ptr<StoreApiClient> client) 
    : storeClient(client) {
    
    // Initialize category expansions based on LLM-instructions.txt
    categoryExpansions["snacks"] = {"chips", "cookies", "granola bars", "crackers", "pretzels"};
    categoryExpansions["dairy"] = {"milk", "cheese", "yogurt", "butter", "cream"};
    categoryExpansions["beverages"] = {"water", "juice", "soda", "coffee", "tea"};
    categoryExpansions["cleaning"] = {"dish soap", "laundry detergent", "bleach", "wipes", "cleaner"};
    categoryExpansions["personal care"] = {"shampoo", "soap", "toothpaste", "deodorant", "lotion"};
    categoryExpansions["baby"] = {"diapers", "wipes", "formula", "baby food", "shampoo"};
}

void LLMInterface::addCategoryExpansion(const std::string& category, const std::vector<std::string>& products) {
    categoryExpansions[category] = products;
}

std::string LLMInterface::detectIntent(const std::string& query) {
    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);
    
    // Detect specific product queries
    if (lowerQuery.find("find") != std::string::npos || 
        lowerQuery.find("search") != std::string::npos ||
        lowerQuery.find("look for") != std::string::npos) {
        return "SEARCH";
    }
    
    // Detect comparison queries
    if (lowerQuery.find("compare") != std::string::npos || 
        lowerQuery.find("cheapest") != std::string::npos ||
        lowerQuery.find("best price") != std::string::npos) {
        return "COMPARE";
    }
    
    // Detect shopping list queries
    if (lowerQuery.find("list") != std::string::npos || 
        lowerQuery.find("buy") != std::string::npos ||
        lowerQuery.find("need") != std::string::npos ||
        lowerQuery.find("get me") != std::string::npos) {
        return "SHOPPING_LIST";
    }
    
    // Detect budget queries
    if (lowerQuery.find("budget") != std::string::npos || 
        lowerQuery.find("spend") != std::string::npos ||
        lowerQuery.find("cost") != std::string::npos ||
        lowerQuery.find("under") != std::string::npos) {
        return "BUDGET";
    }
    
    return "GENERIC";
}

bool LLMInterface::isSpecificQuery(const std::string& query) {
    // Check if query contains specific brand names or detailed product descriptions
    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);
    
    std::vector<std::string> specificIndicators = {
        "samsung", "apple", "lg", "sony", "coca-cola", "coke", "pepsi",
        "tide", "dawn", "pampers", "huggies", "2l", "500ml", "oz", "inch"
    };
    
    for (const auto& indicator : specificIndicators) {
        if (lowerQuery.find(indicator) != std::string::npos) {
            return true;
        }
    }
    
    return false;
}

bool LLMInterface::isGenericQuery(const std::string& query) {
    return !isSpecificQuery(query);
}

std::vector<std::string> LLMInterface::expandCategory(const std::string& category) {
    std::string lowerCategory = category;
    std::transform(lowerCategory.begin(), lowerCategory.end(), lowerCategory.begin(), ::tolower);
    
    // Check if we have a predefined expansion
    if (categoryExpansions.find(lowerCategory) != categoryExpansions.end()) {
        return categoryExpansions[lowerCategory];
    }
    
    // Return the category itself if no expansion available
    return {category};
}

std::string LLMInterface::normalizeProductName(const std::string& productName) {
    std::string normalized = productName;
    
    // Common normalizations
    std::map<std::string, std::string> normalizations = {
        {"coke", "Coca-Cola"},
        {"tv", "Television"},
        {"phone", "Smartphone"},
        {"laptop", "Notebook Computer"}
    };
    
    std::string lowerNormalized = normalized;
    std::transform(lowerNormalized.begin(), lowerNormalized.end(), lowerNormalized.begin(), ::tolower);
    
    for (const auto& [key, value] : normalizations) {
        if (lowerNormalized.find(key) != std::string::npos) {
            return value;
        }
    }
    
    return normalized;
}

std::vector<LLMInterface::RankedResult> LLMInterface::rankByCheapestMix(const std::vector<Item>& items) {
    std::vector<RankedResult> results;
    
    // Group items by product name
    std::map<std::string, std::vector<Item>> productGroups;
    for (const auto& item : items) {
        productGroups[item.getItemName()].push_back(item);
    }
    
    // For each product, find the cheapest option
    std::vector<Item> cheapestItems;
    double totalCost = 0.0;
    
    for (auto& [productName, productItems] : productGroups) {
        auto cheapest = std::min_element(productItems.begin(), productItems.end(),
            [](const Item& a, const Item& b) {
                return a.getCurrentPrice() < b.getCurrentPrice();
            });
        
        if (cheapest != productItems.end()) {
            cheapestItems.push_back(*cheapest);
            totalCost += cheapest->getCurrentPrice();
        }
    }
    
    RankedResult result;
    result.items = cheapestItems;
    result.totalCost = totalCost;
    result.store = "Mixed";
    result.score = totalCost;
    
    results.push_back(result);
    return results;
}

std::vector<LLMInterface::RankedResult> LLMInterface::rankBySingleStore(const std::vector<Item>& items) {
    std::vector<RankedResult> results;
    
    // Group items by store
    std::map<std::string, std::vector<Item>> storeGroups;
    for (const auto& item : items) {
        storeGroups[item.getStore()].push_back(item);
    }
    
    // For each store, calculate total cost
    for (const auto& [store, storeItems] : storeGroups) {
        RankedResult result;
        result.store = store;
        result.items = storeItems;
        result.totalCost = 0.0;
        
        for (const auto& item : storeItems) {
            result.totalCost += item.getCurrentPrice();
        }
        
        result.score = result.totalCost;
        results.push_back(result);
    }
    
    // Sort by total cost (lowest first)
    std::sort(results.begin(), results.end(), 
        [](const RankedResult& a, const RankedResult& b) {
            return a.totalCost < b.totalCost;
        });
    
    return results;
}

std::string LLMInterface::processNaturalLanguageQuery(const std::string& query, Mode mode) {
    std::cout << "[LLM] Processing query: " << query << std::endl;
    
    // Detect intent
    std::string intent = detectIntent(query);
    std::cout << "[LLM] Intent detected: " << intent << std::endl;
    
    // Process based on intent
    if (intent == "SEARCH" || intent == "COMPARE" || intent == "SHOPPING_LIST") {
        // Extract products from query
        std::vector<std::string> products;
        
        if (isGenericQuery(query)) {
            // Expand generic categories
            std::cout << "[LLM] Generic query detected, expanding categories..." << std::endl;
            
            // Simple keyword extraction (in production, use NLP)
            for (const auto& [category, expansion] : categoryExpansions) {
                if (query.find(category) != std::string::npos) {
                    products = expansion;
                    break;
                }
            }
        } else {
            // Specific query - normalize and search
            std::cout << "[LLM] Specific query detected" << std::endl;
            products.push_back(normalizeProductName(query));
        }
        
        // Search for products
        std::vector<Item> allItems;
        for (const auto& product : products) {
            auto items = storeClient->searchAllStores(product);
            allItems.insert(allItems.end(), items.begin(), items.end());
        }
        
        // Format response based on mode
        return formatResponse(allItems, mode);
    }
    
    return "I'm sorry, I couldn't process that query. Please try rephrasing.";
}

std::vector<Item> LLMInterface::generateShoppingList(const std::string& request) {
    std::cout << "[LLM] Generating shopping list for: " << request << std::endl;
    
    // Process the request and return items
    // This would integrate with the natural language processing
    std::vector<Item> shoppingList;
    
    // TODO: Implement smart shopping list generation based on:
    // - User preferences
    // - Budget constraints
    // - Historical purchases
    // - Seasonal deals
    
    return shoppingList;
}

std::string LLMInterface::getBudgetInsight(const std::vector<Item>& items) {
    if (items.empty()) {
        return "No items to analyze.";
    }
    
    double total = 0.0;
    double cheapestOption = std::numeric_limits<double>::max();
    std::string cheapestStore;
    
    // Calculate totals by store
    std::map<std::string, double> storeTotals;
    for (const auto& item : items) {
        storeTotals[item.getStore()] += item.getCurrentPrice();
        total += item.getCurrentPrice();
    }
    
    // Find cheapest store option
    for (const auto& [store, storeTotal] : storeTotals) {
        if (storeTotal < cheapestOption) {
            cheapestOption = storeTotal;
            cheapestStore = store;
        }
    }
    
    std::ostringstream insight;
    insight << std::fixed << std::setprecision(2);
    insight << "Budget Insight:\n";
    insight << "- Total items: " << items.size() << "\n";
    insight << "- Average price per item: $" << (total / items.size()) << "\n";
    insight << "- Cheapest single-store option: " << cheapestStore << " ($" << cheapestOption << ")\n";
    insight << "- Potential savings: $" << (total - cheapestOption) << " by shopping at " << cheapestStore;
    
    return insight.str();
}

std::string LLMInterface::formatTableResponse(const std::vector<Item>& items) {
    if (items.empty()) {
        return "No items found matching your criteria.";
    }
    
    std::ostringstream response;
    response << std::fixed << std::setprecision(2);
    response << "\n| Store     | Item                          | Price   | Notes              |\n";
    response << "|-----------|-------------------------------|---------|--------------------|\n";
    
    for (const auto& item : items) {
        response << "| " << std::left << std::setw(9) << item.getStore() << " | ";
        response << std::setw(29) << item.getItemName().substr(0, 29) << " | $";
        response << std::setw(6) << std::right << item.getCurrentPrice() << " | ";
        response << std::left << std::setw(18) << "In stock" << " |\n";
    }
    
    return response.str();
}

std::string LLMInterface::formatResponse(const std::vector<Item>& items, Mode mode) {
    if (items.empty()) {
        return "No data available for your query.";
    }
    
    std::ostringstream response;
    response << std::fixed << std::setprecision(2);
    
    switch (mode) {
        case Mode::CHEAPEST_MIX: {
            auto ranked = rankByCheapestMix(items);
            if (!ranked.empty()) {
                response << "Here are the cheapest options across all stores:\n";
                response << formatTableResponse(ranked[0].items);
                response << "\nTotal: $" << ranked[0].totalCost;
            }
            break;
        }
        
        case Mode::SINGLE_STORE: {
            auto ranked = rankBySingleStore(items);
            if (!ranked.empty()) {
                response << "Best single-store option: " << ranked[0].store << "\n";
                response << formatTableResponse(ranked[0].items);
                response << "\nTotal: $" << ranked[0].totalCost;
            }
            break;
        }
        
        case Mode::BUDGET_INSIGHT: {
            response << getBudgetInsight(items);
            break;
        }
    }
    
    return response.str();
}
