#include "LLMInterface.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <cctype>
#include <cstdlib>
#include <set>

// Enable SSL support for HTTPS connections to GitHub API
#define CPPHTTPLIB_OPENSSL_SUPPORT

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
    if (envKey != nullptr && strlen(envKey) > 0) {
        openaiApiKey = envKey;
        std::cout << "[LLM] GitHub API token loaded from environment (length: " 
                  << strlen(envKey) << " chars)" << std::endl;
    } else {
        std::cout << "[LLM] Warning: GITHUB_TOKEN not found. Set it with setOpenAIKey() or environment variable." << std::endl;
        std::cout << "[LLM] Debug: envKey pointer = " << (void*)envKey << std::endl;
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
        
        // Create HTTPS client for GitHub Models API with SSL support
        httplib::SSLClient cli("models.github.ai");
        cli.set_connection_timeout(10, 0);  // 10 seconds
        cli.set_read_timeout(30, 0);        // 30 seconds
        cli.enable_server_certificate_verification(true);
        
        // Build request body
        json requestBody = {
            {"model", gptModel},
            {"messages", json::array({
                {
                    {"role", "system"},
                    {"content", "You are Budgie, a helpful shopping assistant for Budgeteer, a price comparison app. "
                               "Your role is to assist users with product, pricing, and shopping inquiries. Follow these guidelines:\n\n"
                               "1. Understand and interpret user queries to identify product names, stores, and budget constraints.\n"
                               "2. Provide concise, accurate, and context-aware responses that prioritize user satisfaction.\n"
                               "3. Depending on the user's needs, either:\n"
                               "   - Send a clear text response to clarify their intent or request additional details.\n"
                               "   - Generate a list of items based on the user's input, starting with generic options if specifics are not provided "
                               "(e.g., suggest common ingredients like flour, sugar, and eggs for a cake recipe if only 'cake ingredients' are requested).\n"
                               "4. Enable users to edit generated lists by adding or removing items as directed.\n"
                               "5. Use generic products available in common stores such as Walmart, Costco, and Loblaws, and avoid suggesting items unlikely to be locally accessible.\n\n"
                               "Focus on clarity, user satisfaction, and adherence to the provided query context and constraints."}
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
            {"Authorization", "Bearer " + openaiApiKey}
        };
        
        // Make POST request to GitHub Models API
        // Let the library set Content-Type via the 4th parameter
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

std::vector<Item> LLMInterface::cherryPickRelevantItems(const std::string& query, const std::vector<Item>& items) {
    // OPTIMIZATION: If we already have a manageable number of items, skip GPT cherry-picking
    if (items.size() <= 20) {
        std::cout << "[LLM] Only " << items.size() << " items, skipping cherry-pick" << std::endl;
        return items;
    }
    
    if (!canMakeGPTRequest()) {
        std::cout << "[LLM] Query limit reached, skipping cherry-pick filtering" << std::endl;
        // Return top 20 items as fallback
        std::vector<Item> fallback(items.begin(), items.begin() + std::min(20, (int)items.size()));
        return fallback;
    }
    
    try {
        // Build a list of unique product names from the search results
        std::set<std::string> uniqueNames;
        for (const auto& item : items) {
            uniqueNames.insert(item.getItemName());
        }
        
        // OPTIMIZATION: Reduced from 100 to 50 for token efficiency
        // Create a compact list of products for GPT to review
        std::ostringstream productList;
        int count = 0;
        for (const auto& name : uniqueNames) {
            if (count >= 50) break;  // Reduced limit for token efficiency
            productList << (count + 1) << ". " << name << "\n";
            count++;
        }
        
        // Build cherry-picking prompt
        std::ostringstream cherryPickPrompt;
        cherryPickPrompt << "User's original query: \"" << query << "\"\n\n";
        cherryPickPrompt << "I found " << std::min((int)uniqueNames.size(), 50) << " unique products. ";
        cherryPickPrompt << "Please select ONLY the products that are DIRECTLY relevant to the user's query.\n\n";
        cherryPickPrompt << "Rules:\n";
        cherryPickPrompt << "1. Only include products that match the user's intent\n";
        cherryPickPrompt << "2. Exclude unrelated products (e.g., exclude 'Apple Watch' when user wants 'apples')\n";
        cherryPickPrompt << "3. For shopping lists, select 8-15 diverse items that fulfill the request\n";
        cherryPickPrompt << "4. Prioritize variety and common grocery items\n\n";
        cherryPickPrompt << "Available products:\n" << productList.str() << "\n";
        cherryPickPrompt << "IMPORTANT: Return ONLY a raw JSON array of product names. Do NOT wrap it in markdown code blocks.\n";
        cherryPickPrompt << "Format: [\"Product Name 1\", \"Product Name 2\", ...]\n";
        cherryPickPrompt << "Your response must start with [ and end with ].";
        
        std::cout << "[LLM] Asking GPT to cherry-pick relevant items..." << std::endl;
        std::string gptResponse = callGPTAPI(cherryPickPrompt.str());
        
        if (gptResponse.empty()) {
            std::cout << "[LLM] Cherry-pick failed, returning top 20 items" << std::endl;
            std::vector<Item> fallback(items.begin(), items.begin() + std::min(20, (int)items.size()));
            return fallback;
        }
        
        // Clean response
        std::string cleanedResponse = gptResponse;
        
        // Remove markdown code blocks
        size_t startPos = cleanedResponse.find("```json");
        if (startPos != std::string::npos) {
            cleanedResponse = cleanedResponse.substr(startPos + 7);
        } else {
            startPos = cleanedResponse.find("```");
            if (startPos != std::string::npos) {
                cleanedResponse = cleanedResponse.substr(startPos + 3);
            }
        }
        
        size_t endPos = cleanedResponse.rfind("```");
        if (endPos != std::string::npos) {
            cleanedResponse = cleanedResponse.substr(0, endPos);
        }
        
        // Trim whitespace
        size_t first = cleanedResponse.find_first_not_of(" \n\r\t");
        if (first == std::string::npos) {
            cleanedResponse = "";
        } else {
            size_t last = cleanedResponse.find_last_not_of(" \n\r\t");
            cleanedResponse = cleanedResponse.substr(first, last - first + 1);
        }
        
        std::cout << "[LLM] Cherry-pick response: " << cleanedResponse.substr(0, 100) << "..." << std::endl;
        
        // Parse the JSON array
        json selectedNames = json::parse(cleanedResponse);
        
        if (!selectedNames.is_array()) {
            std::cerr << "[LLM] Cherry-pick response is not an array" << std::endl;
            std::vector<Item> fallback(items.begin(), items.begin() + std::min(20, (int)items.size()));
            return fallback;
        }
        
        // Convert to list for partial matching
        std::vector<std::string> selectedList;
        for (const auto& name : selectedNames) {
            if (name.is_string()) {
                selectedList.push_back(name.get<std::string>());
            }
        }
        
        // Filter items based on selected names (use partial matching)
        std::vector<Item> filteredItems;
        for (const auto& item : items) {
            std::string itemName = item.getItemName();
            std::string lowerItemName = itemName;
            std::transform(lowerItemName.begin(), lowerItemName.end(), lowerItemName.begin(), ::tolower);
            
            // Check if any selected name is contained in this item's name
            for (const auto& selectedName : selectedList) {
                std::string lowerSelectedName = selectedName;
                std::transform(lowerSelectedName.begin(), lowerSelectedName.end(), lowerSelectedName.begin(), ::tolower);
                
                // Check if the selected name is a substring of the item name
                // or if the item name starts with the selected name
                if (lowerItemName.find(lowerSelectedName) != std::string::npos ||
                    lowerSelectedName.find(lowerItemName) != std::string::npos) {
                    filteredItems.push_back(item);
                    break;  // Found a match, no need to check other selected names
                }
            }
        }
        
        std::cout << "[LLM] Filtered " << items.size() << " items down to " << filteredItems.size() << std::endl;
        
        return filteredItems;
        
    } catch (const std::exception& e) {
        std::cerr << "[LLM] Error in cherry-pick: " << e.what() << std::endl;
        // Return top 20 items as fallback
        std::vector<Item> fallback(items.begin(), items.begin() + std::min(20, (int)items.size()));
        return fallback;
    }
}

LLMInterface::ReasoningResult LLMInterface::reasonAboutShoppingList(const std::string& originalQuery, const std::vector<std::string>& currentItems) {
    ReasoningResult result;
    result.isComplete = false;
    
    if (!canMakeGPTRequest()) {
        std::cout << "[LLM] Query limit reached, skipping reasoning step" << std::endl;
        result.isComplete = true;  // Assume complete if we can't reason
        result.reasoning = "Query limit reached, unable to perform reasoning validation.";
        return result;
    }
    
    try {
        // Build reasoning prompt
        std::ostringstream reasoningPrompt;
        reasoningPrompt << "User's original request: \"" << originalQuery << "\"\n\n";
        reasoningPrompt << "Current shopping list:\n";
        // OPTIMIZATION: Limit list size in prompt to save tokens
        int maxItemsToShow = std::min(30, (int)currentItems.size());
        for (int i = 0; i < maxItemsToShow; i++) {
            reasoningPrompt << (i + 1) << ". " << currentItems[i] << "\n";
        }
        if (currentItems.size() > maxItemsToShow) {
            reasoningPrompt << "... and " << (currentItems.size() - maxItemsToShow) << " more items\n";
        }
        reasoningPrompt << "\n";
        reasoningPrompt << "Task: Analyze if this shopping list logically satisfies the user's request.\n\n";
        reasoningPrompt << "Consider:\n";
        reasoningPrompt << "1. Does the user's request imply a specific recipe or purpose? (e.g., 'cake ingredients' implies baking)\n";
        reasoningPrompt << "2. Are there essential items missing that would typically be needed? (e.g., eggs, flour, sugar for a cake)\n";
        reasoningPrompt << "3. Are there any items that don't belong or seem unnecessary?\n";
        reasoningPrompt << "4. Is there reasonable variety and completeness?\n";
        reasoningPrompt << "5. Suggest max 3-4 missing items if needed\n\n";
        reasoningPrompt << "IMPORTANT: Return ONLY a raw JSON object. Do NOT wrap it in markdown code blocks.\n";
        reasoningPrompt << "Format:\n";
        reasoningPrompt << "{\n";
        reasoningPrompt << "  \"is_complete\": true/false,\n";
        reasoningPrompt << "  \"reasoning\": \"brief explanation of your analysis\",\n";
        reasoningPrompt << "  \"missing_items\": [\"item1\", \"item2\", ...],\n";
        reasoningPrompt << "  \"unnecessary_items\": [\"item3\", \"item4\", ...]\n";
        reasoningPrompt << "}\n\n";
        reasoningPrompt << "Your response must start with { and end with }.";
        
        std::cout << "[LLM] Reasoning about shopping list completeness..." << std::endl;
        std::string gptResponse = callGPTAPI(reasoningPrompt.str());
        
        if (gptResponse.empty()) {
            std::cout << "[LLM] Reasoning failed, assuming list is complete" << std::endl;
            result.isComplete = true;
            result.reasoning = "Unable to validate - assuming list is complete.";
            return result;
        }
        
        // Clean response
        std::string cleanedResponse = gptResponse;
        
        // Remove markdown code blocks
        size_t startPos = cleanedResponse.find("```json");
        if (startPos != std::string::npos) {
            cleanedResponse = cleanedResponse.substr(startPos + 7);
        } else {
            startPos = cleanedResponse.find("```");
            if (startPos != std::string::npos) {
                cleanedResponse = cleanedResponse.substr(startPos + 3);
            }
        }
        
        size_t endPos = cleanedResponse.rfind("```");
        if (endPos != std::string::npos) {
            cleanedResponse = cleanedResponse.substr(0, endPos);
        }
        
        // Trim whitespace
        size_t first = cleanedResponse.find_first_not_of(" \n\r\t");
        if (first == std::string::npos) {
            cleanedResponse = "";
        } else {
            size_t last = cleanedResponse.find_last_not_of(" \n\r\t");
            cleanedResponse = cleanedResponse.substr(first, last - first + 1);
        }
        
        std::cout << "[LLM] Reasoning response: " << cleanedResponse.substr(0, 150) << "..." << std::endl;
        
        // Parse JSON response
        json parsed = json::parse(cleanedResponse);
        
        result.isComplete = parsed.value("is_complete", false);
        result.reasoning = parsed.value("reasoning", "No reasoning provided");
        
        if (parsed.contains("missing_items") && parsed["missing_items"].is_array()) {
            for (const auto& item : parsed["missing_items"]) {
                if (item.is_string()) {
                    result.missingItems.push_back(item.get<std::string>());
                }
            }
        }
        
        if (parsed.contains("unnecessary_items") && parsed["unnecessary_items"].is_array()) {
            for (const auto& item : parsed["unnecessary_items"]) {
                if (item.is_string()) {
                    result.unnecessaryItems.push_back(item.get<std::string>());
                }
            }
        }
        
        std::cout << "[LLM] Reasoning result: " << (result.isComplete ? "Complete" : "Incomplete") << std::endl;
        std::cout << "[LLM] Missing items: " << result.missingItems.size() << ", Unnecessary items: " << result.unnecessaryItems.size() << std::endl;
        
        return result;
        
    } catch (const std::exception& e) {
        std::cerr << "[LLM] Error in reasoning: " << e.what() << std::endl;
        result.isComplete = true;  // Assume complete on error
        result.reasoning = "Error during reasoning validation.";
        return result;
    }
}

std::vector<Item> LLMInterface::refineShoppingListWithReasoning(const std::string& query, std::vector<Item> initialItems, int maxIterations) {
    std::cout << "[LLM] Starting reasoning-based refinement (max " << maxIterations << " iterations)..." << std::endl;
    
    std::vector<Item> currentItems = initialItems;
    std::set<std::string> currentItemNames;
    
    for (const auto& item : currentItems) {
        currentItemNames.insert(item.getItemName());
    }
    
    for (int iteration = 0; iteration < maxIterations; iteration++) {
        std::cout << "[LLM] Reasoning iteration " << (iteration + 1) << "/" << maxIterations << std::endl;
        
        // Convert current items to name list
        std::vector<std::string> nameList;
        for (const auto& name : currentItemNames) {
            nameList.push_back(name);
        }
        
        // Reason about the current list
        ReasoningResult reasoning = reasonAboutShoppingList(query, nameList);
        
        std::cout << "[LLM] Reasoning: " << reasoning.reasoning << std::endl;
        
        if (reasoning.isComplete && reasoning.missingItems.empty() && reasoning.unnecessaryItems.empty()) {
            std::cout << "[LLM] List is complete after " << (iteration + 1) << " iteration(s)" << std::endl;
            break;
        }
        
        bool listModified = false;
        
        // Remove unnecessary items
        if (!reasoning.unnecessaryItems.empty()) {
            std::cout << "[LLM] Removing " << reasoning.unnecessaryItems.size() << " unnecessary items..." << std::endl;
            for (const auto& unnecessaryItem : reasoning.unnecessaryItems) {
                if (currentItemNames.erase(unnecessaryItem) > 0) {
                    std::cout << "[LLM]   - Removed: " << unnecessaryItem << std::endl;
                    listModified = true;
                }
            }
        }
        
        // Add missing items
        if (!reasoning.missingItems.empty()) {
            std::cout << "[LLM] Searching for " << reasoning.missingItems.size() << " missing items..." << std::endl;
            for (const auto& missingItem : reasoning.missingItems) {
                // Check if already in the list
                if (currentItemNames.count(missingItem) > 0) {
                    std::cout << "[LLM]   - Already have: " << missingItem << std::endl;
                    continue;
                }
                
                // Search for the missing item
                auto searchResults = storeClient->searchAllStores(missingItem);
                if (!searchResults.empty()) {
                    // Find the best match by checking if the item name contains the search term
                    // This prevents "flour" from matching "Enfamil Formula" or "sugar" from matching "iPad Air"
                    Item* bestMatch = nullptr;
                    std::string lowerMissingItem = missingItem;
                    std::transform(lowerMissingItem.begin(), lowerMissingItem.end(), 
                                 lowerMissingItem.begin(), ::tolower);
                    
                    // Look for exact or close match in item name
                    for (auto& result : searchResults) {
                        std::string lowerItemName = result.getItemName();
                        std::transform(lowerItemName.begin(), lowerItemName.end(), 
                                     lowerItemName.begin(), ::tolower);
                        
                        // Check if item name starts with or contains the search term as a word
                        bool isGoodMatch = false;
                        
                        // Check if it starts with the search term
                        if (lowerItemName.find(lowerMissingItem) == 0) {
                            isGoodMatch = true;
                        }
                        // Check if search term appears as a complete word
                        else if (lowerItemName.find(" " + lowerMissingItem + " ") != std::string::npos ||
                                 lowerItemName.find(" " + lowerMissingItem) == lowerItemName.length() - lowerMissingItem.length() - 1) {
                            isGoodMatch = true;
                        }
                        // For short search terms (like "eggs"), check more carefully
                        else if (lowerMissingItem.length() <= 5 && lowerItemName.find(lowerMissingItem) != std::string::npos) {
                            // Make sure it's not part of a larger word
                            size_t pos = lowerItemName.find(lowerMissingItem);
                            bool beforeOk = (pos == 0 || lowerItemName[pos-1] == ' ' || lowerItemName[pos-1] == '(');
                            bool afterOk = (pos + lowerMissingItem.length() >= lowerItemName.length() || 
                                          lowerItemName[pos + lowerMissingItem.length()] == ' ' ||
                                          lowerItemName[pos + lowerMissingItem.length()] == ')' ||
                                          lowerItemName[pos + lowerMissingItem.length()] == '(');
                            if (beforeOk && afterOk) {
                                isGoodMatch = true;
                            }
                        }
                        
                        if (isGoodMatch) {
                            bestMatch = &result;
                            break;  // Take first good match
                        }
                    }
                    
                    // If we found a good match, use it; otherwise skip this item
                    if (bestMatch != nullptr) {
                        currentItems.push_back(*bestMatch);
                        currentItemNames.insert(bestMatch->getItemName());
                        std::cout << "[LLM]   + Added: " << bestMatch->getItemName() << std::endl;
                        listModified = true;
                    } else {
                        std::cout << "[LLM]   ✗ No good match found for: " << missingItem << " (skipping)" << std::endl;
                    }
                }
            }
        }
        
        // If no modifications were made, break to avoid infinite loop
        if (!listModified) {
            std::cout << "[LLM] No modifications made in this iteration, stopping refinement" << std::endl;
            break;
        }
    }
    
    // Filter the final item list to only include items with names in currentItemNames
    std::vector<Item> finalItems;
    for (const auto& item : currentItems) {
        if (currentItemNames.count(item.getItemName()) > 0) {
            finalItems.push_back(item);
        }
    }
    
    std::cout << "[LLM] Refinement complete. Final list has " << currentItemNames.size() << " unique items" << std::endl;
    
    // Perform final validation to catch any items that shouldn't be on the list
    finalItems = validateFinalList(query, finalItems);
    
    return finalItems;
}

std::vector<Item> LLMInterface::validateFinalList(const std::string& query, const std::vector<Item>& items) {
    std::cout << "[LLM] Performing final validation check on the list..." << std::endl;
    
    if (items.empty()) {
        std::cout << "[LLM] List is empty, skipping validation" << std::endl;
        return items;
    }
    
    if (!canMakeGPTRequest()) {
        std::cout << "[LLM] Query limit reached, skipping final validation" << std::endl;
        return items;
    }
    
    try {
        // Extract unique item names
        std::vector<std::string> itemNames;
        std::set<std::string> seenNames;
        for (const auto& item : items) {
            if (seenNames.insert(item.getItemName()).second) {
                itemNames.push_back(item.getItemName());
            }
        }
        
        // Build validation prompt
        std::ostringstream validationPrompt;
        validationPrompt << "User's original request: \"" << query << "\"\n\n";
        validationPrompt << "Final shopping list to validate:\n";
        for (size_t i = 0; i < itemNames.size(); i++) {
            validationPrompt << (i + 1) << ". " << itemNames[i] << "\n";
        }
        validationPrompt << "\n";
        validationPrompt << "Task: Perform a final validation check. Are there ANY items on this list that are OBVIOUSLY wrong or completely unrelated to the user's request?\n\n";
        validationPrompt << "Rules:\n";
        validationPrompt << "1. ONLY remove items that are CLEARLY wrong (e.g., 'MacBook' when user wants party snacks, 'Diapers' when user wants cake ingredients)\n";
        validationPrompt << "2. Keep items that are even remotely reasonable or could be interpreted as related\n";
        validationPrompt << "3. When in doubt, KEEP the item - be lenient, not strict\n";
        validationPrompt << "4. Only flag obvious mistakes like completely wrong categories (electronics for food, etc.)\n\n";
        validationPrompt << "IMPORTANT: Return ONLY a raw JSON object. Do NOT wrap it in markdown code blocks.\n";
        validationPrompt << "Format:\n";
        validationPrompt << "{\n";
        validationPrompt << "  \"items_to_remove\": [\"Item Name 1\", \"Item Name 2\", ...],\n";
        validationPrompt << "  \"reason\": \"brief explanation of why these items were removed\"\n";
        validationPrompt << "}\n\n";
        validationPrompt << "If all items are valid, return: {\"items_to_remove\": [], \"reason\": \"All items are valid\"}\n";
        validationPrompt << "Your response must start with { and end with }.";
        
        std::cout << "[LLM] Asking GPT to validate final list..." << std::endl;
        std::string gptResponse = callGPTAPI(validationPrompt.str());
        
        if (gptResponse.empty()) {
            std::cout << "[LLM] Validation failed, returning original list" << std::endl;
            return items;
        }
        
        // Clean response
        std::string cleanedResponse = gptResponse;
        
        // Remove markdown code blocks
        size_t startPos = cleanedResponse.find("```json");
        if (startPos != std::string::npos) {
            cleanedResponse = cleanedResponse.substr(startPos + 7);
        } else {
            startPos = cleanedResponse.find("```");
            if (startPos != std::string::npos) {
                cleanedResponse = cleanedResponse.substr(startPos + 3);
            }
        }
        
        size_t endPos = cleanedResponse.rfind("```");
        if (endPos != std::string::npos) {
            cleanedResponse = cleanedResponse.substr(0, endPos);
        }
        
        // Trim whitespace
        size_t first = cleanedResponse.find_first_not_of(" \n\r\t");
        if (first == std::string::npos) {
            cleanedResponse = "";
        } else {
            size_t last = cleanedResponse.find_last_not_of(" \n\r\t");
            cleanedResponse = cleanedResponse.substr(first, last - first + 1);
        }
        
        std::cout << "[LLM] Validation response: " << cleanedResponse.substr(0, 150) << "..." << std::endl;
        
        // Parse JSON response
        json parsed = json::parse(cleanedResponse);
        
        std::vector<std::string> itemsToRemove;
        if (parsed.contains("items_to_remove") && parsed["items_to_remove"].is_array()) {
            for (const auto& item : parsed["items_to_remove"]) {
                if (item.is_string()) {
                    itemsToRemove.push_back(item.get<std::string>());
                }
            }
        }
        
        std::string reason = parsed.value("reason", "No reason provided");
        std::cout << "[LLM] Validation reason: " << reason << std::endl;
        
        if (itemsToRemove.empty()) {
            std::cout << "[LLM] All items passed validation!" << std::endl;
            return items;
        }
        
        // Remove invalid items
        std::cout << "[LLM] Removing " << itemsToRemove.size() << " invalid items..." << std::endl;
        std::set<std::string> removeSet(itemsToRemove.begin(), itemsToRemove.end());
        
        std::vector<Item> validatedItems;
        for (const auto& item : items) {
            if (removeSet.find(item.getItemName()) == removeSet.end()) {
                validatedItems.push_back(item);
            } else {
                std::cout << "[LLM]   - Removed: " << item.getItemName() << std::endl;
            }
        }
        
        std::cout << "[LLM] Final validation complete. " << validatedItems.size() << " items remaining." << std::endl;
        return validatedItems;
        
    } catch (const std::exception& e) {
        std::cerr << "[LLM] Error in final validation: " << e.what() << std::endl;
        return items;  // Return original list on error
    }
}

std::string LLMInterface::buildPrompt(const std::string& query, const std::string& context) {
    std::ostringstream prompt;
    prompt << "User query: " << query << "\n\n";
    
    if (!context.empty()) {
        prompt << "Context: " << context << "\n\n";
    }
    
    prompt << "Please analyze this query and provide:\n\n";
    prompt << "1. Intent (search/compare/shopping_list/budget)\n";
    prompt << "2. SPECIFIC product names (not search phrases)\n";
    prompt << "3. Any budget constraints\n";
    prompt << "4. Store preferences if mentioned\n\n";
    prompt << "CRITICAL for search_terms:\n";
    prompt << "- Return SPECIFIC PRODUCT NAMES, not generic search phrases\n";
    prompt << "- Example: For 'fruits', return ['apples', 'bananas', 'oranges', 'grapes', 'strawberries']\n";
    prompt << "- Example: For 'snacks', return ['chips', 'cookies', 'pretzels', 'crackers', 'popcorn']\n";
    prompt << "- Example: For 'cake ingredients', return ['flour', 'sugar', 'eggs', 'butter', 'baking powder', 'vanilla extract']\n";
    prompt << "- DO NOT return phrases like 'fresh fruits', 'buy snacks', 'snack ideas'\n";
    prompt << "- Return 5-10 common specific items that match the category\n\n";
    prompt << "IMPORTANT: Return ONLY a raw JSON object. Do NOT wrap it in markdown code blocks or use ```json. ";
    prompt << "Your response must start with { and end with }.\n\n";
    prompt << "Format: {\"intent\": \"...\", \"products\": [...], \"budget\": null or number, \"stores\": [...], \"search_terms\": [list of specific product names]}";
    
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
        // Strip markdown code blocks if present (```json ... ``` or ``` ... ```)
        std::string cleanedResponse = gptResponse;
        
        // Remove opening code block
        size_t startPos = cleanedResponse.find("```json");
        if (startPos != std::string::npos) {
            // Found ```json, skip it and the following newline
            cleanedResponse = cleanedResponse.substr(startPos + 7);
        } else {
            startPos = cleanedResponse.find("```");
            if (startPos != std::string::npos) {
                // Found ```, skip it and the following newline
                cleanedResponse = cleanedResponse.substr(startPos + 3);
            }
        }
        
        // Remove closing code block
        size_t endPos = cleanedResponse.rfind("```");
        if (endPos != std::string::npos) {
            cleanedResponse = cleanedResponse.substr(0, endPos);
        }
        
        // Trim all whitespace (including newlines, tabs, spaces)
        size_t first = cleanedResponse.find_first_not_of(" \n\r\t");
        if (first == std::string::npos) {
            cleanedResponse = "";
        } else {
            size_t last = cleanedResponse.find_last_not_of(" \n\r\t");
            cleanedResponse = cleanedResponse.substr(first, last - first + 1);
        }
        
        std::cout << "[LLM] Cleaned JSON: " << cleanedResponse.substr(0, 100) << "..." << std::endl;
        
        // Parse GPT response
        json parsed = json::parse(cleanedResponse);
        
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
        
        // Cherry-pick relevant items using GPT
        std::cout << "[LLM] Found " << allItems.size() << " items, cherry-picking relevant ones..." << std::endl;
        std::vector<Item> filteredItems = cherryPickRelevantItems(query, allItems);
        
        if (filteredItems.empty()) {
            return "I couldn't find any products that specifically match your query. Try being more specific or use different keywords.";
        }
        
        std::cout << "[LLM] Cherry-picked " << filteredItems.size() << " relevant items" << std::endl;
        
        // Apply reasoning-based refinement for complex queries that might need logical completion
        // (e.g., "cake ingredients" should include flour, eggs, sugar, etc.)
        std::string lowerQuery = query;
        std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);
        
        bool needsReasoning = (lowerQuery.find("ingredients") != std::string::npos ||
                              lowerQuery.find("recipe") != std::string::npos ||
                              lowerQuery.find("make a") != std::string::npos ||
                              lowerQuery.find("bake") != std::string::npos ||
                              lowerQuery.find("cook") != std::string::npos ||
                              lowerQuery.find("prepare") != std::string::npos);
        
        if (needsReasoning) {
            std::cout << "[LLM] Query requires logical reasoning - refining list..." << std::endl;
            filteredItems = refineShoppingListWithReasoning(query, filteredItems, 3);
        }
        
        // Format response
        return formatResponse(filteredItems, mode);
        
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
    
    // Use the natural language query processing to generate items
    if (useGPT && !openaiApiKey.empty()) {
        std::cout << "[LLM] Using GPT-4o-mini to generate shopping list..." << std::endl;
        
        try {
            // Build a prompt specifically for shopping list generation
            std::ostringstream prompt;
            prompt << "User's shopping request: \"" << request << "\"\n\n";
            prompt << "Generate a shopping list based on this request. Consider:\n";
            prompt << "1. What items are needed based on the request\n";
            prompt << "2. Budget constraints mentioned (if any)\n";
            prompt << "3. Common grocery items for the scenario\n";
            prompt << "4. Variety and practicality\n\n";
            prompt << "Available stores: Walmart, Loblaws, Costco\n\n";
            prompt << "IMPORTANT: Return ONLY a raw JSON object. Do NOT wrap it in markdown code blocks.\n";
            prompt << "Format:\n";
            prompt << "{\n";
            prompt << "  \"items\": [\"item1\", \"item2\", \"item3\", ...],\n";
            prompt << "  \"reasoning\": \"brief explanation of your selection\"\n";
            prompt << "}\n\n";
            prompt << "Your response must start with { and end with }.";
            
            std::string gptResponse = callGPTAPI(prompt.str());
            
            if (gptResponse.empty()) {
                std::cout << "[LLM] GPT call failed, falling back to local processing" << std::endl;
                return generateShoppingListLocally(request);
            }
            
            // Clean response
            std::string cleanedResponse = gptResponse;
            
            // Remove markdown code blocks
            size_t startPos = cleanedResponse.find("```json");
            if (startPos != std::string::npos) {
                cleanedResponse = cleanedResponse.substr(startPos + 7);
            } else {
                startPos = cleanedResponse.find("```");
                if (startPos != std::string::npos) {
                    cleanedResponse = cleanedResponse.substr(startPos + 3);
                }
            }
            
            size_t endPos = cleanedResponse.rfind("```");
            if (endPos != std::string::npos) {
                cleanedResponse = cleanedResponse.substr(0, endPos);
            }
            
            // Trim whitespace
            size_t first = cleanedResponse.find_first_not_of(" \n\r\t");
            if (first == std::string::npos) {
                cleanedResponse = "";
            } else {
                size_t last = cleanedResponse.find_last_not_of(" \n\r\t");
                cleanedResponse = cleanedResponse.substr(first, last - first + 1);
            }
            
            std::cout << "[LLM] Shopping list response: " << cleanedResponse.substr(0, 150) << "..." << std::endl;
            
            // Parse JSON response
            json parsed = json::parse(cleanedResponse);
            
            if (!parsed.contains("items") || !parsed["items"].is_array()) {
                std::cerr << "[LLM] Invalid response format, falling back" << std::endl;
                return generateShoppingListLocally(request);
            }
            
            // Extract item names from GPT response
            std::vector<std::string> itemNames;
            for (const auto& item : parsed["items"]) {
                if (item.is_string()) {
                    itemNames.push_back(item.get<std::string>());
                }
            }
            
            std::cout << "[LLM] GPT suggested " << itemNames.size() << " items" << std::endl;
            
            // Search for each item in the database
            std::vector<Item> shoppingList;
            for (const auto& itemName : itemNames) {
                auto searchResults = storeClient->searchAllStores(itemName);
                
                if (!searchResults.empty()) {
                    // Find the cheapest option for this item
                    auto cheapest = std::min_element(searchResults.begin(), searchResults.end(),
                        [](const Item& a, const Item& b) {
                            return a.getCurrentPrice() < b.getCurrentPrice();
                        });
                    
                    if (cheapest != searchResults.end()) {
                        shoppingList.push_back(*cheapest);
                        std::cout << "[LLM]   + Added: " << cheapest->getItemName() 
                                 << " ($" << cheapest->getCurrentPrice() << " at " 
                                 << cheapest->getStore() << ")" << std::endl;
                    }
                } else {
                    std::cout << "[LLM]   - Not found: " << itemName << std::endl;
                }
            }
            
            if (shoppingList.empty()) {
                std::cout << "[LLM] No items found in database, trying fallback" << std::endl;
                return generateShoppingListLocally(request);
            }
            
            std::cout << "[LLM] Generated shopping list with " << shoppingList.size() << " items" << std::endl;
            
            // Perform final validation to ensure no inappropriate items made it through
            shoppingList = validateFinalList(request, shoppingList);
            
            return shoppingList;
            
        } catch (const std::exception& e) {
            std::cerr << "[LLM] Error generating shopping list: " << e.what() << std::endl;
            return generateShoppingListLocally(request);
        }
    } else {
        std::cout << "[LLM] GPT disabled or no API key, using local processing" << std::endl;
        return generateShoppingListLocally(request);
    }
}

std::vector<Item> LLMInterface::generateShoppingListLocally(const std::string& request) {
    std::cout << "[LLM] Generating shopping list locally..." << std::endl;
    
    // Extract keywords from request
    std::string lowerRequest = request;
    std::transform(lowerRequest.begin(), lowerRequest.end(), lowerRequest.begin(), ::tolower);
    
    std::vector<Item> shoppingList;
    
    // Simple keyword-based matching
    std::map<std::string, std::vector<std::string>> scenarios = {
        {"snack", {"chips", "cookies", "soda", "candy"}},
        {"party", {"chips", "soda", "cookies", "pizza"}},
        {"breakfast", {"eggs", "milk", "bread", "butter", "cereal"}},
        {"lunch", {"bread", "cheese", "meat", "lettuce"}},
        {"dinner", {"chicken", "rice", "pasta", "sauce"}},
        {"cake", {"flour", "sugar", "eggs", "butter", "milk"}},
        {"pasta", {"pasta", "sauce", "cheese", "garlic"}},
        {"groceries", {"milk", "bread", "eggs", "butter"}}
    };
    
    // Find matching scenario
    std::vector<std::string> searchTerms;
    for (const auto& [keyword, items] : scenarios) {
        if (lowerRequest.find(keyword) != std::string::npos) {
            searchTerms = items;
            std::cout << "[LLM] Matched scenario: " << keyword << std::endl;
            break;
        }
    }
    
    // If no scenario matched, try to extract products from the request
    if (searchTerms.empty()) {
        // Just use the request as-is
        searchTerms.push_back(request);
    }
    
    // Search for each term
    for (const auto& term : searchTerms) {
        auto results = storeClient->searchAllStores(term);
        
        if (!results.empty()) {
            // Find cheapest option
            auto cheapest = std::min_element(results.begin(), results.end(),
                [](const Item& a, const Item& b) {
                    return a.getCurrentPrice() < b.getCurrentPrice();
                });
            
            if (cheapest != results.end()) {
                shoppingList.push_back(*cheapest);
                std::cout << "[LLM]   + Added: " << cheapest->getItemName() << std::endl;
            }
        }
    }
    
    std::cout << "[LLM] Local generation found " << shoppingList.size() << " items" << std::endl;
    
    // Perform final validation if GPT is available
    if (useGPT && !openaiApiKey.empty()) {
        shoppingList = validateFinalList(request, shoppingList);
    }
    
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
