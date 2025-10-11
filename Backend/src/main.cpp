/**
 * @file main.cpp
 * @brief Entry point for the Budgeteer Backend API Server
 * 
 * This file initializes and starts the Budgeteer API server in either:
 * - HTTP server mode (for production/frontend integration)
 * - CLI interactive mode (for testing and development)
 * 
 * The server provides price comparison and AI-powered shopping assistance
 * using a sample dataset and optional GPT-4o-mini integration via GitHub Models API.
 * 
 * @author York Entrepreneurship Competition Team
 * @date October 2025
 * @version 1.0
 */

#include "ApiServer.h"
#include <iostream>
#include <string>

/**
 * @brief Main application entry point
 * 
 * Parses command-line arguments, initializes the API server with the database,
 * and starts the server in the requested mode (HTTP or CLI).
 * 
 * @param argc Number of command-line arguments
 * @param argv Array of command-line argument strings
 * @return int Exit code (0 = success, 1 = initialization failure)
 * 
 * Command-line options:
 *   --http, -h          Start in HTTP server mode (default: CLI mode)
 *   --port, -p <num>    Set server port (default: 8080)
 *   --help              Display help message
 * 
 * Example usage:
 *   ./BudgeteerAPI                  # CLI mode with sample dataset
 *   ./BudgeteerAPI --http           # HTTP server on port 8080
 *   ./BudgeteerAPI --http --port 3000  # HTTP server on custom port
 */
int main(int argc, char* argv[]) {
    // Display welcome banner
    std::cout << "========================================\n";
    std::cout << "    Budgeteer Backend API Server\n";
    std::cout << "    with LLM & Sample Database\n";
    std::cout << "========================================\n\n";
    
    // Initialize default configuration
    bool httpMode = false;  // Default to CLI mode
    int port = 8080;        // Default HTTP port
    
    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        // Check for HTTP server mode flag
        if (arg == "--http" || arg == "-h") {
            httpMode = true;
        } 
        // Check for port configuration
        else if (arg == "--port" || arg == "-p") {
            if (i + 1 < argc) {
                port = std::stoi(argv[++i]);
            }
        } 
        // Display help information
        else if (arg == "--help") {
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
    
    // Database file path (relative to executable location)
    // This CSV file contains sample product data from Walmart, Loblaws, and Costco
    std::string dbPath = "SampleDataset/yec_competition_dataset.csv";
    
    // Create API server instance
    // Parameters:
    //   - dbPath: Path to CSV database file
    //   - port: HTTP server port number
    //   - useRealTimeApis: false (always use local database, real-time APIs removed)
    ApiServer server(dbPath, port, false);
    
    // Initialize server and load database from CSV file
    // This step loads all product data into memory for fast querying
    if (!server.initialize()) {
        std::cerr << "Failed to initialize server. Exiting...\n";
        return 1;  // Exit with error code
    }
    
    // Display server configuration summary
    std::cout << "\n========================================\n";
    std::cout << "Server ready to handle requests!\n";
    std::cout << "Port: " << port << "\n";
    std::cout << "Mode: " << (httpMode ? "HTTP Server" : "CLI Interactive") << "\n";
    std::cout << "Data Source: Sample Dataset\n";
    std::cout << "========================================\n";
    
    // Start the server in the appropriate mode
    if (httpMode) {
        // HTTP Server Mode: Start REST API server for frontend integration
        // This will block until the server is stopped (Ctrl+C)
        server.startHttpServer();
    } else {
        // CLI Interactive Mode: Start command-line interface for testing
        // User can interactively test API functionality
        server.run();
    }
    
    return 0;  // Successful execution
}
