# GPT-4o-mini Quick Reference

## Setup (30 seconds)

```powershell
# 1. Set API key
$env:OPENAI_API_KEY="sk-your-key-here"

# 2. Build
cd Backend\build
cmake .. ; cmake --build . --config Debug

# 3. Run
cd bin\Debug
.\BudgeteerAPI.exe
```

## Code Examples

### Basic Usage
```cpp
#include "LLMInterface.h"

auto llm = std::make_shared<LLMInterface>(storeClient);

// Process query (automatically uses GPT or local)
std::string response = llm->processNaturalLanguageQuery(
    "Find cheap snacks for a party",
    LLMInterface::Mode::CHEAPEST_MIX
);
```

### Configuration
```cpp
// Manual API key
llm->setOpenAIKey("sk-your-key-here");

// Change model
llm->setGPTModel("gpt-4o-mini");

// Set limits
llm->setDailyQueryLimit(500);

// Disable GPT (local only)
llm->enableGPTMode(false);
```

### Processing Modes
```cpp
// Mode 1: Cheapest across all stores
Mode::CHEAPEST_MIX

// Mode 2: Best single store
Mode::SINGLE_STORE

// Mode 3: Budget insights
Mode::BUDGET_INSIGHT
```

## Query Types

| Query | Processing | Cost |
|-------|-----------|------|
| "Find Samsung TV" | Local | Free |
| "Price of milk" | Local | Free |
| "Snacks under $50" | GPT | ~$0.0001 |
| "Party supplies on budget" | GPT | ~$0.0001 |

## Features

✅ Auto-detection (simple vs complex)  
✅ Automatic fallback to local  
✅ Rate limiting built-in  
✅ JSON response parsing  
✅ Error handling  

## Costs

- **Dev/Testing:** ~$0.23/month
- **Production (1K queries/day):** ~$2.25/month
- **With hybrid:** 70% cheaper

## Environment Variables

```powershell
# Required
$env:OPENAI_API_KEY="sk-..."

# Optional
$env:GPT_MODEL="gpt-4o-mini"
$env:DAILY_QUERY_LIMIT="1000"
```

## Logs to Watch

```
[LLM] OpenAI API key loaded from environment ✓
[LLM] Using model: gpt-4o-mini ✓
[LLM] Simple query detected, using local processing
[LLM] Complex query detected, using GPT-4o-mini
[LLM] GPT response received (query 42/1000)
[LLM] Daily query limit reached (1000)
[LLM] GPT failed, falling back to local processing
```

## Common Issues

| Error | Solution |
|-------|----------|
| "OPENAI_API_KEY not found" | Set environment variable |
| "Daily query limit reached" | Increase limit or wait |
| "Connection failed" | Check internet/firewall |
| SSL errors | Install OpenSSL |

## Files Modified

- ✅ `CMakeLists.txt` - Added httplib & json
- ✅ `LLMInterface.h` - Added GPT methods
- ✅ `LLMInterface.cpp` - Implemented GPT integration
- ✅ `.env.example` - Configuration template
- ✅ `README.md` - Updated documentation

## Security Checklist

- [ ] Never commit API keys
- [ ] Use environment variables
- [ ] Add `.env` to `.gitignore`
- [ ] Rotate keys every 90 days
- [ ] Monitor usage dashboard

## Testing

```cpp
// Test without API key (local only)
llm->enableGPTMode(false);
auto result = llm->processNaturalLanguageQuery("test");

// Test with GPT
llm->enableGPTMode(true);
llm->setOpenAIKey("sk-...");
auto result = llm->processNaturalLanguageQuery("complex query");
```

## Performance

- **Local processing:** <1ms
- **GPT-4o-mini:** 200-500ms
- **Hybrid avg:** ~100ms

## Full Documentation

See **[GPT_INTEGRATION.md](GPT_INTEGRATION.md)** for complete guide.
