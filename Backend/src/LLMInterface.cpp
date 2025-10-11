#include "LLMInterface.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <cctype>
#include <cstdlib>

#ifdef _WIN32
    #define CPPHTTPLIB_OPENSSL_SUPPORT
#endif

#include <httplib.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

LLMInterface::LLMInterface(std::shared_ptr<StoreApiClient> client) 
    : storeClient(client),
      useGPT(true),  // Enable GPT by default
      gptModel("openai/gpt-4o-mini"),  // Use GPT-4o-mini via GitHub
      maxTokens(500),
      temperature(0.7),
      dailyQueryCount(0),
      dailyQueryLimit(1000) {
    
    // Try to get API key from environment variable (GitHub token)
    const char* envKey = std::getenv("GITHUB_TOKEN");
    if (envKey != nullptr) {
        openaiApiKey = envKey;
        std::cout << "[LLM] GitHub API token loaded from environment" << std::endl;
    } else {
        std::cout << "[LLM] Warning: GITHUB_TOKEN not found. Set it with setOpenAIKey() or environment variable." << std::endl;
        useGPT = false;  // Disable GPT if no key
    }
    
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

// Configuration methods
void LLMInterface::setOpenAIKey(const std::string& key) {
    openaiApiKey = key;
    if (!key.empty()) {
        useGPT = true;
        std::cout << "[LLM] GitHub API token configured" << std::endl;
    }
}

void LLMInterface::enableGPTMode(bool enable) {
    useGPT = enable && !openaiApiKey.empty();
    std::cout << "[LLM] GPT mode: " << (useGPT ? "enabled" : "disabled") << std::endl;
}

void LLMInterface::setDailyQueryLimit(int limit) {
    dailyQueryLimit = limit;
}

void LLMInterface::setGPTModel(const std::string& model) {
    gptModel = model;
}

std::string LLMInterface::getGPTModel() const {
    return gptModel;
}

bool LLMInterface::canMakeGPTRequest() {
    if (dailyQueryCount >= dailyQueryLimit) {
        std::cout << "[LLM] Daily query limit reached (" << dailyQueryLimit << ")" << std::endl;
        return false;
    }
    return true;
}

// GPT API Integration
std::string LLMInterface::callGPTAPI(const std::string& prompt) {
    if (!canMakeGPTRequest()) {
        return "";
    }
    
    try {
        std::cout << "[LLM] Calling GPT-4o-mini via GitHub Models API..." << std::endl;
        
        // Create HTTPS client for GitHub Models API
        httplib::SSLClient cli("models.github.ai");
        cli.set_connection_timeout(10, 0);  // 10 seconds
        cli.set_read_timeout(30, 0);        // 30 seconds
        
        // Build request body
        json requestBody = {
            {"model", gptModel},
            {"messages", json::array({
                {
                    {"role", "system"},
                    {"content", "You are a helpful shopping assistant for Budgeteer, a price comparison app. "
                               "Your job is to understand user queries about products, prices, and shopping needs. "
                               "Be concise, accurate, and helpful. Focus on extracting product names, stores, and budget constraints."}
                },
                {
                    {"role", "user"},
                    {"content", prompt}
                }
            })},
            {"max_tokens", maxTokens},
            {"temperature", temperature}
        };
        
        std::string body = requestBody.dump();
        
        // Set headers for GitHub API
        httplib::Headers headers = {
            {"Content-Type", "application/json"},
            {"Authorization", "Bearer " + openaiApiKey}
        };
        
        // Make POST request to GitHub Models API
        auto res = cli.Post("/inference/chat/completions", headers, body, "application/json");
        
        if (res && res->status == 200) {
            json response = json::parse(res->body);
            std::string content = response["choices"][0]["message"]["content"];
            
            dailyQueryCount++;
            std::cout << "[LLM] GPT response received (query " << dailyQueryCount 
                     << "/" << dailyQueryLimit << ")" << std::endl;
            
            return content;
        } else if (res) {
            std::cerr << "[LLM] GitHub API Error: " << res->status << " - " << res->body << std::endl;
        } else {
            std::cerr << "[LLM] Connection failed to GitHub Models API" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "[LLM] Exception calling GitHub API: " << e.what() << std::endl;
    }
    
    return "";
}

std::string LLMInterface::buildPrompt(const std::string& query, const std::string& context) {
    std::ostringstream prompt;
    prompt << "User query: \"" << query << "\"\n\n";
    
    if (!context.empty()) {
        prompt << "Context: " << context << "\n\n";
    }
    
    prompt << "Please analyze this query and provide:\n";
    prompt << "1. Intent (search/compare/shopping_list/budget)\n";
    prompt << "2. Product names or categories mentioned\n";
    prompt << "3. Any budget constraints\n";
    prompt << "4. Store preferences if mentioned\n";
    prompt << "5. Suggested search terms for our database\n\n";
    prompt << "Format your response as JSON with keys: intent, products, budget, stores, search_terms";
    
    return prompt.str();
}

bool LLMInterface::isSimpleQuery(const std::string& query) {
    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);
    
    // Simple queries are direct product searches
    std::vector<std::string> simpleIndicators = {
        "find", "search", "price of", "how much"
    };
    
    // Check if it's a short, direct query
    if (query.length() < 30) {
        for (const auto& indicator : simpleIndicators) {
            if (lowerQuery.find(indicator) != std::string::npos) {
                return true;
            }
        }
    }
    
    // Check for specific product names (likely simple)
    if (isSpecificQuery(query) && query.length() < 50) {
        return true;
    }
    
    return false;
}

std::string LLMInterface::detectIntentLocal(const std::string& query) {
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

std::string LLMInterface::processQueryWithGPT(const std::string& query, Mode mode) {
    std::cout << "[LLM] Processing with GPT-4o-mini via GitHub..." << std::endl;
    
    // Build context-aware prompt
    std::string prompt = buildPrompt(query, "Available stores: Walmart, Loblaws, Costco");
    
    // Call GPT API
    std::string gptResponse = callGPTAPI(prompt);
    
    if (gptResponse.empty()) {
        std::cout << "[LLM] GPT failed, falling back to local processing" << std::endl;
        return processQueryLocally(query, mode);
    }
    
    try {
        // Parse GPT response
        json parsed = json::parse(gptResponse);
        
        // Extract search terms
        std::vector<std::string> searchTerms;
        if (parsed.contains("search_terms") && parsed["search_terms"].is_array()) {
            for (const auto& term : parsed["search_terms"]) {
                searchTerms.push_back(term.get<std::string>());
            }
        } else if (parsed.contains("products") && parsed["products"].is_array()) {
            for (const auto& product : parsed["products"]) {
                searchTerms.push_back(product.get<std::string>());
            }
        } else {
            // Fallback: use original query
            searchTerms.push_back(query);
        }
        
        // Search for products
        std::vector<Item> allItems;
        for (const auto& term : searchTerms) {
            auto items = storeClient->searchAllStores(term);
            allItems.insert(allItems.end(), items.begin(), items.end());
        }
        
        if (allItems.empty()) {
            return "I couldn't find any products matching your query. Try being more specific or use different keywords.";
        }
        
        // Format response
        return formatResponse(allItems, mode);
        
    } catch (const json::exception& e) {
        std::cerr << "[LLM] Error parsing GPT response: " << e.what() << std::endl;
        std::cerr << "[LLM] Raw response: " << gptResponse << std::endl;
        // Fallback to local processing
        return processQueryLocally(query, mode);
    }
}

std::string LLMInterface::processQueryLocally(const std::string& query, Mode mode) {
    std::cout << "[LLM] Processing locally (fallback mode)..." << std::endl;
    
    // Detect intent
    std::string intent = detectIntentLocal(query);
    std::cout << "[LLM] Intent detected: " << intent << std::endl;
    
    // Extract products from query
    std::vector<std::string> products;
    
    if (isGenericQuery(query)) {
        // Expand generic categories
        std::cout << "[LLM] Generic query detected, expanding categories..." << std::endl;
        
        // Simple keyword extraction
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
    
    if (products.empty()) {
        products.push_back(query);
    }
    
    // Search for products
    std::vector<Item> allItems;
    for (const auto& product : products) {
        auto items = storeClient->searchAllStores(product);
        allItems.insert(allItems.end(), items.begin(), items.end());
    }
    
    if (allItems.empty()) {
        return "No products found matching your query.";
    }
    
    // Format response based on mode
    return formatResponse(allItems, mode);
}

std::string LLMInterface::processNaturalLanguageQuery(const std::string& query, Mode mode) {
    std::cout << "[LLM] Processing query: " << query << std::endl;
    std::cout << "[LLM] Using model: " << gptModel << " via GitHub" << std::endl;
    
    // Decide whether to use GPT or local processing
    if (useGPT && !openaiApiKey.empty()) {
        // Use GPT for complex queries, local for simple ones (hybrid approach)
        if (isSimpleQuery(query)) {
            std::cout << "[LLM] Simple query detected, using local processing" << std::endl;
            return processQueryLocally(query, mode);
        } else {
            std::cout << "[LLM] Complex query detected, using GPT-4o-mini via GitHub" << std::endl;
            return processQueryWithGPT(query, mode);
        }
    } else {
        std::cout << "[LLM] GPT disabled or no GitHub token, using local processing" << std::endl;
        return processQueryLocally(query, mode);
    }
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
