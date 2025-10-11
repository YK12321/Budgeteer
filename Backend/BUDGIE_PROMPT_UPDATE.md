# GPT System Prompt Update - Budgie Assistant

## Changes Made

### 1. Updated System Prompt
The GPT assistant is now called "Budgie" and has enhanced capabilities for shopping assistance.

**New System Prompt:**
```
You are Budgie, a helpful shopping assistant for Budgeteer, a price comparison app. 
Your role is to assist users with product, pricing, and shopping inquiries. Follow these guidelines:

1. Understand and interpret user queries to identify product names, stores, and budget constraints.
2. Provide concise, accurate, and context-aware responses that prioritize user satisfaction.
3. Depending on the user's needs, either:
   - Send a clear text response to clarify their intent or request additional details.
   - Generate a list of items based on the user's input, starting with generic options if specifics 
     are not provided (e.g., suggest common ingredients like flour, sugar, and eggs for a cake recipe 
     if only 'cake ingredients' are requested).
4. Enable users to edit generated lists by adding or removing items as directed.
5. Use generic products available in common stores such as Walmart, Costco, and Loblaws, and avoid 
   suggesting items unlikely to be locally accessible.

Focus on clarity, user satisfaction, and adherence to the provided query context and constraints.
```

### 2. Enhanced User Prompt
Updated the prompt building to be more explicit about JSON formatting requirements.

**Key Changes:**
- Added explicit instruction: "Return ONLY a raw JSON object"
- Emphasized: "Do NOT wrap it in markdown code blocks or use ```json"
- Specified exact format: Response must start with `{` and end with `}`
- Provided example format

**New Prompt Structure:**
```
User query: [query]

Context: Available stores: Walmart, Loblaws, Costco

Please analyze this query and provide:

1. Intent (search/compare/shopping_list/budget)
2. Product names or categories mentioned
3. Any budget constraints
4. Store preferences if mentioned
5. Suggested search terms for our database

IMPORTANT: Return ONLY a raw JSON object. Do NOT wrap it in markdown code blocks or use ```json. 
Your response must start with { and end with }.

Format: {"intent": "...", "products": [...], "budget": null or number, "stores": [...], "search_terms": [...]}
```

### 3. Added Markdown Code Block Stripper
Even with explicit instructions, GPT sometimes wraps JSON in markdown. Added fallback handling:

```cpp
// Strip markdown code blocks if present (```json ... ```)
std::string cleanedResponse = gptResponse;
size_t startPos = cleanedResponse.find("```json");
if (startPos != std::string::npos) {
    cleanedResponse = cleanedResponse.substr(startPos + 7);  // Skip ```json
} else {
    startPos = cleanedResponse.find("```");
    if (startPos != std::string::npos) {
        cleanedResponse = cleanedResponse.substr(startPos + 3);  // Skip ```
    }
}

size_t endPos = cleanedResponse.rfind("```");
if (endPos != std::string::npos) {
    cleanedResponse = cleanedResponse.substr(0, endPos);
}

// Trim whitespace
cleanedResponse.erase(0, cleanedResponse.find_first_not_of(" \n\r\t"));
cleanedResponse.erase(cleanedResponse.find_last_not_of(" \n\r\t") + 1);
```

This ensures JSON parsing succeeds even if GPT disobeys formatting instructions.

## Example Use Cases

### Use Case 1: Cake Ingredients
**User Query:** "I want to buy the ingredients for a cake"

**Expected GPT Response:**
```json
{
  "intent": "shopping_list",
  "products": ["flour", "sugar", "eggs", "butter", "baking powder", "vanilla extract", "milk"],
  "budget": null,
  "stores": ["Walmart", "Loblaws", "Costco"],
  "search_terms": ["all-purpose flour", "granulated sugar", "eggs", "unsalted butter", 
                   "baking powder", "vanilla extract", "milk"]
}
```

**Budgie's Behavior:**
- Identifies this as a shopping list request
- Expands "cake ingredients" to common baking items
- Provides generic products available at major stores
- Searches for each ingredient in the database

### Use Case 2: Fruits for a Party
**User Query:** "I need to buy fruits for a party"

**Expected GPT Response:**
```json
{
  "intent": "shopping_list",
  "products": ["apples", "bananas", "oranges", "grapes", "strawberries", "watermelon"],
  "budget": null,
  "stores": ["Walmart", "Loblaws", "Costco"],
  "search_terms": ["fresh apples", "bananas", "oranges", "grapes", "strawberries", 
                   "watermelon", "fruit platter", "fruit tray"]
}
```

**Budgie's Behavior:**
- Expands generic "fruits" to variety of common fruits
- Suggests party-appropriate quantities and types
- Provides options from all available stores

### Use Case 3: Budget Constraint
**User Query:** "I need snacks under $20"

**Expected GPT Response:**
```json
{
  "intent": "budget",
  "products": ["chips", "cookies", "crackers", "popcorn", "granola bars"],
  "budget": 20,
  "stores": ["Walmart", "Loblaws", "Costco"],
  "search_terms": ["chips", "cookies", "crackers", "popcorn", "granola bars"]
}
```

**Budgie's Behavior:**
- Identifies budget constraint ($20)
- Suggests variety of common snacks
- Filters results to stay within budget
- Shows cheapest options first

## Benefits

1. **Better Product Expansion**: Budgie now intelligently expands generic requests into specific product lists
2. **Context Awareness**: Understands when user wants clarification vs. when to generate a list
3. **Store-Appropriate Suggestions**: Only suggests items commonly available in target stores
4. **Robust JSON Parsing**: Handles GPT responses even when formatting isn't perfect
5. **User-Friendly**: Named assistant ("Budgie") creates better user experience

## Testing

To test the new prompt:

```powershell
# Start the API
cd build\bin\Debug
.\BudgeteerAPI.exe

# Test queries:
# 1. Generic expansion
curl -X POST http://localhost:8080/api/llm/query -H "Content-Type: application/json" -d '{"query": "I want to buy the ingredients for a cake"}'

# 2. Fruit for party
curl -X POST http://localhost:8080/api/llm/query -H "Content-Type: application/json" -d '{"query": "I need fruits for a party"}'

# 3. Budget constraint
curl -X POST http://localhost:8080/api/llm/query -H "Content-Type: application/json" -d '{"query": "I need snacks under $20"}'

# 4. Store preference
curl -X POST http://localhost:8080/api/llm/query -H "Content-Type: application/json" -d '{"query": "What can I buy at Walmart for breakfast?"}'
```

## Files Modified

- `Backend/src/LLMInterface.cpp`:
  - Updated system prompt (line ~105-118)
  - Updated buildPrompt() function (line ~152-170)
  - Added markdown stripping logic (line ~383-400)

## Related Documentation

- `HTTPS_FIX.md` - SSL/HTTPS setup
- `CONTENT_TYPE_FIX.md` - API header fixes
- `OPENSSL_SETUP.md` - OpenSSL installation
- `run-api.ps1` - Helper script to run API with token
- `test-api.ps1` - Test script for API endpoints

## Notes

- The system prompt is now more conversational and user-friendly
- Budgie actively expands generic requests (e.g., "cake ingredients" → specific items)
- Fallback handling ensures robustness even with malformed GPT responses
- Token must be set via `$env:GITHUB_TOKEN` for GPT features to work
