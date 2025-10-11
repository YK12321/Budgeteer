# GPT-4o-mini Integration Guide

## Overview

Budgeteer now uses **GPT-4o-mini** by default for natural language query processing! This provides:
- ✅ Better intent understanding
- ✅ Smarter product extraction
- ✅ Context-aware responses
- ✅ Budget constraint recognition
- ✅ Hybrid local/GPT processing for optimal cost/performance

## Quick Setup

### 1. Get OpenAI API Key

1. Go to [OpenAI Platform](https://platform.openai.com/)
2. Sign up or log in
3. Navigate to **API Keys** section
4. Click **"Create new secret key"**
5. Copy your key (starts with `sk-...`)

### 2. Set Environment Variable

#### Windows (PowerShell)
```powershell
$env:OPENAI_API_KEY="sk-your-api-key-here"
```

#### Windows (Command Prompt)
```cmd
set OPENAI_API_KEY=sk-your-api-key-here
```

#### Linux/Mac
```bash
export OPENAI_API_KEY="sk-your-api-key-here"
```

#### Permanent (Windows)
```powershell
[System.Environment]::SetEnvironmentVariable('OPENAI_API_KEY', 'sk-your-api-key-here', 'User')
```

### 3. Build and Run

```powershell
cd Backend
mkdir build
cd build
cmake ..
cmake --build . --config Debug
cd bin\Debug
.\BudgeteerAPI.exe
```

## Features

### Hybrid Processing (Smart Mode)

The system automatically decides when to use GPT vs local processing:

**Uses Local Processing (Free, Fast):**
- ✅ Simple product searches: "find Samsung TV"
- ✅ Direct queries: "price of milk"
- ✅ Store-specific: "Walmart TVs"

**Uses GPT-4o-mini (Smart, Accurate):**
- 🧠 Complex queries: "I need snacks for a party under $50"
- 🧠 Multi-intent: "Compare prices and suggest alternatives"
- 🧠 Ambiguous: "Need something to clean floors"
- 🧠 Conversational: "What organic options do you have?"

### Query Examples

```cpp
// Simple query → Local processing
"Find Samsung TV"

// Complex query → GPT-4o-mini
"I'm hosting a party for 20 people on a $100 budget. 
 What snacks and beverages should I buy?"

// Budget constraints → GPT-4o-mini
"Get me cleaning supplies under $30, preferably eco-friendly"

// Multi-store comparison → GPT-4o-mini
"Where can I get the cheapest groceries for a week?"
```

## Configuration

### Programmatic Configuration

```cpp
#include "LLMInterface.h"

// Create LLM interface
auto llm = std::make_shared<LLMInterface>(storeClient);

// Set API key manually (if not in environment)
llm->setOpenAIKey("sk-your-api-key-here");

// Configure settings
llm->setGPTModel("gpt-4o-mini");  // Default model
llm->setDailyQueryLimit(1000);    // Limit queries per day
llm->enableGPTMode(true);          // Enable/disable GPT

// Check current model
std::cout << "Using model: " << llm->getGPTModel() << std::endl;

// Process query
std::string response = llm->processNaturalLanguageQuery(
    "Find me cheap snacks",
    LLMInterface::Mode::CHEAPEST_MIX
);
```

### Cost Control

**Daily Query Limit:**
```cpp
llm->setDailyQueryLimit(500);  // Max 500 GPT queries per day
```

**Disable GPT (Use Local Only):**
```cpp
llm->enableGPTMode(false);  // All queries use local processing
```

## Cost Estimation

### GPT-4o-mini Pricing (as of 2024)
- **Input:** ~$0.15 per 1M tokens
- **Output:** ~$0.60 per 1M tokens

### Example Costs

**Development/Testing (100 queries/day for 30 days):**
- Total queries: 3,000
- Avg tokens: 500 per query
- Total tokens: 1.5M
- **Cost: ~$0.23/month**

**Production (1,000 queries/day):**
- Total queries: 30,000/month
- Avg tokens: 500 per query
- Total tokens: 15M
- **Cost: ~$2.25/month**

**Competition Demo (with hybrid approach):**
- Simple queries: 70% → Free (local)
- Complex queries: 30% → GPT
- Effective cost: **~$0.68/month** for 1,000 queries/day

## API Response Format

GPT returns structured JSON:

```json
{
  "intent": "shopping_list",
  "products": ["chips", "soda", "cookies"],
  "budget": 50.00,
  "stores": ["Walmart", "Costco"],
  "search_terms": ["chips", "soda", "cookies", "snacks"]
}
```

## Error Handling

### Automatic Fallback

If GPT fails (network, rate limit, API error), the system automatically falls back to local processing:

```
[LLM] Calling GPT-4o-mini API...
[LLM] API Error: 429 - Rate limit exceeded
[LLM] GPT failed, falling back to local processing
[LLM] Processing locally (fallback mode)...
```

### Common Issues

**Issue: "OPENAI_API_KEY not found"**
- Solution: Set environment variable before running

**Issue: "Daily query limit reached"**
- Solution: Increase limit or wait 24 hours
- Or: Disable GPT temporarily

**Issue: "Connection failed to OpenAI API"**
- Solution: Check internet connection
- Check firewall settings
- Verify API key is valid

**Issue: SSL/TLS errors**
- Solution: Ensure OpenSSL is installed
- Windows: OpenSSL should be in system PATH

## Security Best Practices

### ⚠️ Never Commit API Keys

Add to `.gitignore`:
```
.env
*.key
*secret*
```

### Use Environment Variables

✅ **Good:**
```cpp
const char* key = std::getenv("OPENAI_API_KEY");
```

❌ **Bad:**
```cpp
std::string key = "sk-1234567890abcdef...";  // Never hardcode!
```

### Rotate Keys Regularly

- Rotate API keys every 90 days
- Use separate keys for dev/prod
- Monitor usage in OpenAI dashboard

## Monitoring

### Usage Tracking

```cpp
// Check current usage
std::cout << "Queries today: " << dailyQueryCount 
          << "/" << dailyQueryLimit << std::endl;
```

### Logs

Watch for these log messages:

```
[LLM] OpenAI API key loaded from environment
[LLM] Using model: gpt-4o-mini
[LLM] Simple query detected, using local processing
[LLM] Complex query detected, using GPT-4o-mini
[LLM] GPT response received (query 42/1000)
```

## Performance Tips

### 1. Optimize Prompts

Keep prompts concise to reduce token usage:
```cpp
// Good: Concise prompt
"User query: 'cheap snacks'\nExtract: products, budget"

// Bad: Verbose prompt
"The user has asked about finding affordable snacks..."
```

### 2. Cache Responses

For repeated queries, implement caching:
```cpp
std::map<std::string, std::string> responseCache;
```

### 3. Batch Similar Queries

If processing multiple queries, batch them:
```cpp
// Process shopping list items together
"Products needed: milk, bread, eggs, cheese"
```

## Testing Without API Key

To test without an API key:

```cpp
llm->enableGPTMode(false);  // Disable GPT
// All queries will use local processing
```

Or simply don't set the environment variable.

## Troubleshooting

### Build Issues

**httplib not found:**
```
Solution: CMake will auto-download via FetchContent
Ensure you have internet connection during first build
```

**OpenSSL not found (Windows):**
```powershell
# Install OpenSSL
choco install openssl
# Or download from: https://slproweb.com/products/Win32OpenSSL.html
```

### Runtime Issues

**Check API key:**
```cpp
if (llm->getGPTModel() == "gpt-4o-mini") {
    std::cout << "GPT configured correctly" << std::endl;
}
```

**Test connection:**
```cpp
std::string response = llm->callGPTAPI("Test query");
if (!response.empty()) {
    std::cout << "GPT API working!" << std::endl;
}
```

## Advanced Usage

### Custom Models

Use different OpenAI models:
```cpp
llm->setGPTModel("gpt-4o-mini");      // Fastest, cheapest (default)
llm->setGPTModel("gpt-4o");           // More capable
llm->setGPTModel("gpt-4-turbo");      // Previous generation
```

### Temperature Control

Modify `LLMInterface.cpp`:
```cpp
temperature = 0.3;  // More deterministic
temperature = 0.7;  // Balanced (default)
temperature = 1.0;  // More creative
```

### Token Limits

Modify `LLMInterface.cpp`:
```cpp
maxTokens = 300;   // Shorter responses
maxTokens = 500;   // Default
maxTokens = 1000;  // Longer responses
```

## Future Enhancements

Potential improvements:
- [ ] Streaming responses for real-time feedback
- [ ] Context memory for conversation history
- [ ] Fine-tuned model for shopping queries
- [ ] Multi-language support
- [ ] Voice input integration
- [ ] Response caching system
- [ ] A/B testing framework

## Support

For issues or questions:
1. Check OpenAI API status: https://status.openai.com/
2. Review logs in terminal
3. Test with local mode disabled
4. Verify API key is valid
5. Check OpenAI usage dashboard

## Summary

✅ **GPT-4o-mini enabled by default**  
✅ **Hybrid processing for cost optimization**  
✅ **Automatic fallback to local processing**  
✅ **Easy configuration via environment variables**  
✅ **Built-in rate limiting and error handling**  

The integration is production-ready and will significantly enhance your Budgeteer project for the YEC competition! 🚀
