# Citations and External Resources

This document lists all external libraries, software, tools, and resources used in the Budgeteer project.

## Backend Dependencies

### Core Libraries

#### cpp-httplib
- **Purpose**: HTTP server and client library for C++
- **Version**: Header-only library
- **Repository**: https://github.com/yhirose/cpp-httplib
- **License**: MIT License
- **Usage**: Provides HTTP server functionality for the REST API, handles request routing, CORS, and HTTP client operations

#### nlohmann/json
- **Purpose**: JSON parsing and serialization for C++
- **Version**: 3.x
- **Repository**: https://github.com/nlohmann/json
- **License**: MIT License
- **Usage**: JSON data handling for API responses and GPT API communication

### Build Tools

#### CMake
- **Purpose**: Cross-platform build system generator
- **Version**: 3.10+
- **Website**: https://cmake.org/
- **License**: BSD 3-Clause License
- **Usage**: Build configuration and compilation management

#### Microsoft Visual C++ Compiler (MSVC)
- **Purpose**: C++ compiler for Windows
- **Version**: Visual Studio 2019/2022
- **Website**: https://visualstudio.microsoft.com/
- **License**: Proprietary (Microsoft)
- **Usage**: Compiling C++ backend code on Windows

#### GCC/MinGW
- **Purpose**: Alternative C++ compiler
- **Version**: 7.0+
- **Website**: https://gcc.gnu.org/ / http://mingw-w64.org/
- **License**: GPL (GNU General Public License)
- **Usage**: Alternative compiler option for Windows/Linux builds

### Security Libraries

#### OpenSSL
- **Purpose**: SSL/TLS cryptographic library
- **Version**: 1.1.1 or 3.x
- **Website**: https://www.openssl.org/
- **License**: Apache License 2.0
- **Usage**: HTTPS support for secure API communication with GitHub Models API

## Frontend Dependencies

### None (Vanilla JavaScript)
The frontend is built using vanilla JavaScript, HTML5, and CSS3 without external frameworks or libraries, ensuring lightweight deployment and minimal dependencies.

### Browser APIs Used
- **Fetch API**: For HTTP requests to backend
- **DOM API**: For UI manipulation
- **LocalStorage API**: (Future use for storing preferences)

## External APIs

### GitHub Models API
- **Purpose**: Large Language Model (LLM) integration for natural language processing
- **Model**: GPT-4o-mini (OpenAI model via GitHub)
- **Endpoint**: https://models.github.ai/inference/chat/completions
- **Documentation**: https://docs.github.com/en/github-models
- **Authentication**: GitHub Personal Access Token (PAT)
- **License**: Subject to GitHub Terms of Service and OpenAI usage policies
- **Usage**: 
  - Natural language query processing
  - Shopping list generation
  - Product relevance filtering
  - Budget insights and recommendations

### OpenAI GPT-4o-mini Model
- **Provider**: OpenAI (accessed via GitHub Models)
- **Model**: gpt-4o-mini
- **Documentation**: https://openai.com/index/gpt-4o-mini/
- **Usage**: Natural language understanding and generation for Budgie chatbot

## Development Tools

### Git
- **Purpose**: Version control system
- **Version**: 2.x+
- **Website**: https://git-scm.com/
- **License**: GPL v2
- **Usage**: Source code version control and collaboration

### GitHub
- **Purpose**: Git repository hosting and collaboration platform
- **Website**: https://github.com/
- **Repository**: https://github.com/YK12321/Budgeteer
- **License**: Proprietary platform (free for public repositories)
- **Usage**: Code hosting, version control, and project management

### Visual Studio Code
- **Purpose**: Code editor and IDE
- **Version**: Latest
- **Website**: https://code.visualstudio.com/
- **License**: MIT License (Code - OSS), Proprietary (VS Code binary)
- **Usage**: Primary development environment

### PowerShell
- **Purpose**: Command-line shell and scripting language
- **Version**: 5.1+ (Windows PowerShell) / 7+ (PowerShell Core)
- **Website**: https://docs.microsoft.com/en-us/powershell/
- **License**: MIT License (PowerShell Core), Proprietary (Windows PowerShell)
- **Usage**: Build scripts and automation tasks

## Data Sources

### YEC Competition Dataset
- **Purpose**: Sample product and pricing data
- **Format**: CSV file
- **Location**: `Backend/SampleDataset/yec_competition_dataset.csv`
- **Content**: Product listings from Walmart, Loblaws, and Costco with:
  - Item IDs, names, descriptions
  - Current prices and historical price dates
  - Store information
  - Category tags
  - Image URLs (placeholders)
- **Source**: York Entrepreneurship Competition (YEC) 2025
- **License**: Competition dataset for educational/competition use

## Algorithms and Techniques

### Levenshtein Distance
- **Purpose**: String similarity measurement for fuzzy search
- **Reference**: https://en.wikipedia.org/wiki/Levenshtein_distance
- **Implementation**: Custom implementation in `Database.cpp`
- **Usage**: Product name matching and search relevance scoring

### Search Ranking Algorithm
- **Purpose**: Product search result ranking
- **Techniques Used**:
  - Exact match prioritization
  - Prefix matching
  - Substring matching
  - Fuzzy matching (Levenshtein distance)
  - Word-based partial matching
- **Implementation**: Custom implementation in `Database::searchItems()`

## Standards and Protocols

### HTTP/1.1
- **Purpose**: Web communication protocol
- **RFC**: RFC 7230-7235
- **Website**: https://tools.ietf.org/html/rfc7230
- **Usage**: REST API communication between frontend and backend

### JSON (JavaScript Object Notation)
- **Purpose**: Data interchange format
- **RFC**: RFC 8259
- **Website**: https://www.json.org/
- **Usage**: API request/response format, configuration data

### CORS (Cross-Origin Resource Sharing)
- **Purpose**: Secure cross-origin HTTP requests
- **W3C Recommendation**: https://www.w3.org/TR/cors/
- **Usage**: Enabling frontend-backend communication across different origins

### REST (Representational State Transfer)
- **Purpose**: Architectural style for web APIs
- **Reference**: Roy Fielding's dissertation (2000)
- **Website**: https://www.ics.uci.edu/~fielding/pubs/dissertation/rest_arch_style.htm
- **Usage**: API design pattern for backend endpoints

## Documentation Tools

### Markdown
- **Purpose**: Lightweight markup language for documentation
- **Specification**: CommonMark
- **Website**: https://commonmark.org/
- **Usage**: All project documentation files

## Inspiration and References

### E-commerce Price Comparison Platforms
- **Concept Reference**: Inspired by platforms like:
  - Google Shopping
  - PriceGrabber
  - Shopzilla
- **Note**: No code or proprietary techniques were copied; only general UX concepts

### Conversational AI Shopping Assistants
- **Concept Reference**: Inspired by:
  - Amazon Alexa Shopping
  - Google Assistant Shopping
  - ChatGPT shopping plugins
- **Note**: Implementation is original; inspired by user interaction patterns

## License Summary

This project uses the following license types:
- **MIT License**: cpp-httplib, nlohmann/json, PowerShell Core
- **BSD License**: CMake
- **GPL**: GCC
- **Apache 2.0**: OpenSSL
- **Proprietary**: Microsoft Visual Studio, GitHub Platform, OpenAI Models

All dependencies are used in accordance with their respective licenses. The Budgeteer project itself should be licensed appropriately to comply with these dependencies.

---

**Note**: This document should be kept up-to-date as new dependencies or tools are added to the project. Always verify license compatibility before adding new dependencies.

**Last Updated**: October 11, 2025
