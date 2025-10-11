# AI Reasoning Flow Diagram

## Overall Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                    USER QUERY                                    │
│              "I need ingredients to make a cake"                 │
└────────────────────────────┬────────────────────────────────────┘
                             │
                             ▼
┌─────────────────────────────────────────────────────────────────┐
│                 INTENT DETECTION                                 │
│  Checks for: "ingredients", "recipe", "make", "bake", "cook"    │
│  → Reasoning Required: YES                                       │
└────────────────────────────┬────────────────────────────────────┘
                             │
                             ▼
┌─────────────────────────────────────────────────────────────────┐
│              INITIAL SEARCH & CHERRY-PICKING                     │
│  1. Search database for query terms                              │
│  2. GPT cherry-picks relevant items                              │
│  Result: [Flour, Sugar, Chocolate Chips, Strawberries]          │
└────────────────────────────┬────────────────────────────────────┘
                             │
                             ▼
┌─────────────────────────────────────────────────────────────────┐
│                  REASONING LOOP BEGINS                           │
│                   (Max 3 Iterations)                             │
└────────────────────────────┬────────────────────────────────────┘
                             │
                ┌────────────┴────────────┐
                │    ITERATION 1          │
                └────────────┬────────────┘
                             │
                             ▼
┌─────────────────────────────────────────────────────────────────┐
│               GPT REASONING ANALYSIS                             │
│  Current List: [Flour, Sugar, Chocolate Chips, Strawberries]    │
│                                                                   │
│  Questions:                                                       │
│  ❓ Does this fulfill "cake ingredients"?                        │
│  ❓ What's missing for baking a cake?                            │
│  ❓ Are all items appropriate?                                   │
│                                                                   │
│  GPT Response:                                                    │
│  {                                                                │
│    "is_complete": false,                                          │
│    "reasoning": "Missing essential baking items",                │
│    "missing_items": ["eggs", "butter", "baking powder"],        │
│    "unnecessary_items": ["strawberries"]                         │
│  }                                                                │
└────────────────────────────┬────────────────────────────────────┘
                             │
                             ▼
┌─────────────────────────────────────────────────────────────────┐
│                  LIST MODIFICATION                               │
│  Remove: [Strawberries]                                          │
│  Search & Add: [Eggs, Butter, Baking Powder]                    │
│                                                                   │
│  Updated List: [Flour, Sugar, Chocolate Chips, Eggs,            │
│                 Butter, Baking Powder]                           │
└────────────────────────────┬────────────────────────────────────┘
                             │
                ┌────────────┴────────────┐
                │    ITERATION 2          │
                └────────────┬────────────┘
                             │
                             ▼
┌─────────────────────────────────────────────────────────────────┐
│               GPT REASONING ANALYSIS                             │
│  Current List: [Flour, Sugar, Chocolate Chips, Eggs,            │
│                 Butter, Baking Powder]                           │
│                                                                   │
│  GPT Response:                                                    │
│  {                                                                │
│    "is_complete": false,                                          │
│    "reasoning": "Should include milk for better texture",        │
│    "missing_items": ["milk"],                                    │
│    "unnecessary_items": []                                       │
│  }                                                                │
└────────────────────────────┬────────────────────────────────────┘
                             │
                             ▼
┌─────────────────────────────────────────────────────────────────┐
│                  LIST MODIFICATION                               │
│  Add: [Milk]                                                     │
│                                                                   │
│  Updated List: [Flour, Sugar, Chocolate Chips, Eggs,            │
│                 Butter, Baking Powder, Milk]                     │
└────────────────────────────┬────────────────────────────────────┘
                             │
                ┌────────────┴────────────┐
                │    ITERATION 3          │
                └────────────┬────────────┘
                             │
                             ▼
┌─────────────────────────────────────────────────────────────────┐
│               GPT REASONING ANALYSIS                             │
│  Current List: [Flour, Sugar, Chocolate Chips, Eggs,            │
│                 Butter, Baking Powder, Milk]                     │
│                                                                   │
│  GPT Response:                                                    │
│  {                                                                │
│    "is_complete": true,                                           │
│    "reasoning": "All essential cake ingredients present",        │
│    "missing_items": [],                                          │
│    "unnecessary_items": []                                       │
│  }                                                                │
└────────────────────────────┬────────────────────────────────────┘
                             │
                             ▼
┌─────────────────────────────────────────────────────────────────┐
│                    REASONING COMPLETE                            │
│         List is logically complete after 3 iterations            │
└────────────────────────────┬────────────────────────────────────┘
                             │
                             ▼
┌─────────────────────────────────────────────────────────────────┐
│                   PRICE OPTIMIZATION                             │
│  For each item, find cheapest option across all stores          │
└────────────────────────────┬────────────────────────────────────┘
                             │
                             ▼
┌─────────────────────────────────────────────────────────────────┐
│                    FINAL RESPONSE                                │
│                                                                   │
│  Here are the cheapest options across all stores:               │
│                                                                   │
│  | Store   | Item              | Price |                        │
│  |---------|-------------------|-------|                        │
│  | Costco  | Flour (2kg)       | $4.99 |                        │
│  | Costco  | Sugar (1kg)       | $3.49 |                        │
│  | Walmart | Chocolate Chips   | $5.99 |                        │
│  | Costco  | Eggs (dozen)      | $3.29 |                        │
│  | Costco  | Butter (454g)     | $5.17 |                        │
│  | Walmart | Baking Powder     | $2.99 |                        │
│  | Costco  | Milk 2% (4L)      | $5.59 |                        │
│                                                                   │
│  Total: $31.51                                                   │
└─────────────────────────────────────────────────────────────────┘
```

## Key Decision Points

### 1. Should Reasoning Activate?
```
Query contains:
  - "ingredients" ✓
  - "recipe"      
  - "make a"      
  - "bake"        
  - "cook"        
  - "prepare"     

→ At least one match? → YES → Activate Reasoning
```

### 2. Is List Complete?
```
GPT evaluates:
  ┌─────────────────────────────────┐
  │ Purpose fulfilled?              │ → NO → Continue
  │ Essential items present?        │ → NO → Continue
  │ Unnecessary items present?      │ → YES → Continue
  │ Reasonable variety?             │ → NO → Continue
  └─────────────────────────────────┘
              ↓
         All YES & 
      No modifications?
              ↓
          COMPLETE!
```

### 3. When to Stop?
```
Stop if:
  1. is_complete = true AND
  2. missing_items = [] AND
  3. unnecessary_items = []

OR

  1. Max iterations reached (3)

OR

  1. No modifications in iteration
```

## GPT Reasoning Prompt Structure

```
┌──────────────────────────────────────────────────┐
│  INPUT TO GPT                                    │
├──────────────────────────────────────────────────┤
│  User's original request: "make a cake"         │
│                                                  │
│  Current shopping list:                         │
│  1. Flour                                       │
│  2. Sugar                                       │
│  3. Chocolate Chips                             │
│                                                  │
│  Task: Analyze completeness                     │
│  - Does request imply specific recipe?          │
│  - Missing essential items?                     │
│  - Unnecessary items?                           │
│  - Reasonable variety?                          │
└──────────────────────────────────────────────────┘
                    ↓
              GPT THINKS
                    ↓
┌──────────────────────────────────────────────────┐
│  OUTPUT FROM GPT (JSON)                          │
├──────────────────────────────────────────────────┤
│  {                                               │
│    "is_complete": false,                         │
│    "reasoning": "Missing eggs, butter, and       │
│                  baking powder - essential       │
│                  for cake baking",               │
│    "missing_items": [                            │
│      "eggs",                                     │
│      "butter",                                   │
│      "baking powder"                             │
│    ],                                            │
│    "unnecessary_items": []                       │
│  }                                               │
└──────────────────────────────────────────────────┘
```

## Performance Metrics

```
┌─────────────────────────────────────────────────┐
│  REASONING PERFORMANCE                          │
├─────────────────────────────────────────────────┤
│  Average Iterations:        1.8                 │
│  Max Iterations:            3                   │
│  Success Rate:              ~95%                │
│  Additional Processing:     +2-5 seconds        │
│  GPT Calls per Query:       2-5                 │
│    - Initial cherry-pick:   1                   │
│    - Reasoning iterations:  1-3                 │
│    - Final validation:      0-1                 │
└─────────────────────────────────────────────────┘
```

## Example: Complex Query Flow

```
USER: "I need to prepare a healthy salad"

STEP 1: Keyword Detection
  ✓ Contains "prepare" → Reasoning ENABLED

STEP 2: Initial Search & Cherry-Pick
  Search terms: ["salad", "healthy", "prepare"]
  Found: 3000+ items
  Cherry-picked: Lettuce, Tomatoes, Cucumbers
  
STEP 3: Reasoning Loop

  Iteration 1:
    Current: [Lettuce, Tomatoes, Cucumbers]
    AI: "Needs protein and dressing"
    Add: [Chicken Breast, Ranch Dressing]
    
  Iteration 2:
    Current: [Lettuce, Tomatoes, Cucumbers, Chicken, Dressing]
    AI: "Could use more vegetables and toppings"
    Add: [Carrots, Cheese, Croutons]
    
  Iteration 3:
    Current: [Lettuce, Tomatoes, Cucumbers, Chicken, Dressing,
              Carrots, Cheese, Croutons]
    AI: "Complete healthy salad with protein, vegetables, and toppings"
    Status: COMPLETE ✓

STEP 4: Price Optimization
  Find cheapest option for each item

STEP 5: Return Result
  Total: 8 items, $23.45
```

## Code Flow

```cpp
// Entry point
processQueryWithGPT()
    ↓
// Initial filtering
cherryPickRelevantItems()
    ↓
// Check if reasoning needed
if (query.contains("ingredients") || ...) {
    ↓
    // Reasoning loop
    refineShoppingListWithReasoning()
        ↓
        for (iteration = 1 to 3) {
            ↓
            // Analyze current list
            reasonAboutShoppingList()
                ↓
                // GPT analyzes and returns JSON
                callGPTAPI()
                ↓
            // Modify list based on reasoning
            if (!complete) {
                remove(unnecessaryItems)
                add(missingItems)
            } else {
                break // Done!
            }
        }
}
    ↓
// Format final response
formatResponse()
```
