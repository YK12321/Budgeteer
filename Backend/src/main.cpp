#include "ApiServer.h"
#include <iostream>
#include <string>

int main() {
    std::cout << "========================================\n";
    std::cout << "    Budgeteer Backend API Server\n";
    std::cout << "========================================\n\n";
    
    // Database file path (relative to executable)
    std::string dbPath = "SampleDataset/yec_competition_dataset.csv";
    
    // Create API server instance
    ApiServer server(dbPath, 8080);
    
    // Initialize server and load database
    if (!server.initialize()) {
        std::cerr << "Failed to initialize server. Exiting...\n";
        return 1;
    }
    
    std::cout << "\n========================================\n";
    std::cout << "Server ready to handle requests!\n";
    std::cout << "========================================\n";
    
    // Run the interactive API server
    server.run();
    
    return 0;
}
