#include "ApiServer.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    std::cout << "========================================\n";
    std::cout << "    Budgeteer Backend API Server\n";
    std::cout << "    with LLM & Sample Database\n";
    std::cout << "========================================\n\n";
    
    // Parse command line arguments
    bool httpMode = false;
    int port = 8080;
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--http" || arg == "-h") {
            httpMode = true;
        } else if (arg == "--port" || arg == "-p") {
            if (i + 1 < argc) {
                port = std::stoi(argv[++i]);
            }
        } else if (arg == "--help") {
            std::cout << "Usage: " << argv[0] << " [options]\n\n";
            std::cout << "Options:\n";
            std::cout << "  --http, -h        Start HTTP server mode (requires cpp-httplib)\n";
            std::cout << "  --port, -p <num>  Set server port (default: 8080)\n";
            std::cout << "  --help            Show this help message\n\n";
            std::cout << "Examples:\n";
            std::cout << "  " << argv[0] << "                  # CLI mode with sample dataset\n";
            std::cout << "  " << argv[0] << " --http           # HTTP server mode\n";
            std::cout << "  " << argv[0] << " --http --port 3000  # HTTP server on port 3000\n";
            std::cout << "\nNote: All searches use the sample database. Real-time store APIs are disabled.\n";
            return 0;
        }
    }
    
    // Database file path (relative to executable)
    std::string dbPath = "SampleDataset/yec_competition_dataset.csv";
    
    // Create API server instance (useRealTimeApis=false, always use database)
    ApiServer server(dbPath, port, false);
    
    // Initialize server and load database
    if (!server.initialize()) {
        std::cerr << "Failed to initialize server. Exiting...\n";
        return 1;
    }
    
    std::cout << "\n========================================\n";
    std::cout << "Server ready to handle requests!\n";
    std::cout << "Port: " << port << "\n";
    std::cout << "Mode: " << (httpMode ? "HTTP Server" : "CLI Interactive") << "\n";
    std::cout << "Data Source: Sample Dataset\n";
    std::cout << "========================================\n";
    
    // Run the server
    if (httpMode) {
        server.startHttpServer();
    } else {
        server.run();
    }
    
    return 0;
}
