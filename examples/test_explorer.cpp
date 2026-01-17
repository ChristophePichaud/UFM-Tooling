#include "../include/FileSystemExplorer.h"
#include "../include/SourceExplorer.h"
#include <iostream>
#include <iomanip>

using namespace UFMTooling;

void printSeparator() {
    std::cout << std::string(80, '=') << std::endl;
}

void demonstrateFileSystemExplorer() {
    std::cout << "\n";
    printSeparator();
    std::cout << "FILE SYSTEM EXPLORER EXAMPLE" << std::endl;
    printSeparator();

    FileSystemExplorer fsExplorer;
    
    // Test with examples directory (non-recursive)
    std::cout << "\n1. Non-recursive exploration of 'examples' directory:" << std::endl;
    FileSystemExplorerResult result1 = fsExplorer.explore("examples", false);
    
    if (result1.success) {
        std::cout << "   Found " << result1.entries.size() << " entries" << std::endl;
        for (const auto& entry : result1.entries) {
            std::cout << "   - " << entry.name 
                      << (entry.isDirectory ? " [DIR]" : "") 
                      << std::endl;
        }
    } else {
        std::cout << "   Error: " << result1.errorMessage << std::endl;
    }
    
    // Test with examples directory (recursive)
    std::cout << "\n2. Recursive exploration of 'examples' directory:" << std::endl;
    FileSystemExplorerResult result2 = fsExplorer.explore("examples", true);
    
    if (result2.success) {
        std::cout << "   Found " << result2.entries.size() << " entries" << std::endl;
        
        // Show only .h files
        std::vector<FileSystemEntry> headerFiles = fsExplorer.getFilesByExtension(".h");
        std::cout << "   Header files (.h): " << headerFiles.size() << std::endl;
        for (const auto& entry : headerFiles) {
            std::cout << "   - " << entry.path << std::endl;
        }
    } else {
        std::cout << "   Error: " << result2.errorMessage << std::endl;
    }
}

void demonstrateSourceExplorer() {
    std::cout << "\n";
    printSeparator();
    std::cout << "SOURCE EXPLORER EXAMPLE" << std::endl;
    printSeparator();

    SourceExplorer explorer;
    
    // Explore the examples directory
    std::cout << "\n1. Exploring 'examples' directory for header files:" << std::endl;
    SourceExplorerResult result = explorer.explore("examples", true);
    
    if (result.success) {
        std::cout << "   Files processed: " << result.filesProcessed << std::endl;
        std::cout << "   Files with errors: " << result.filesWithErrors << std::endl;
        
        std::cout << "\n2. Analysis results:" << std::endl;
        for (const auto& analysis : result.analyses) {
            std::cout << "\n   File: " << analysis.filename << std::endl;
            std::cout << "   Path: " << analysis.path << std::endl;
            std::cout << "   Success: " << (analysis.success ? "Yes" : "No") << std::endl;
            
            if (analysis.success) {
                std::cout << "   Classes found: " << analysis.parseResult.classes.size() << std::endl;
                for (const auto& cls : analysis.parseResult.classes) {
                    std::cout << "     - Class: " << cls.name << std::endl;
                    std::cout << "       Members: " << cls.members.size() << std::endl;
                    std::cout << "       Methods: " << cls.methods.size() << std::endl;
                }
                
                std::cout << "   Enums found: " << analysis.parseResult.enums.size() << std::endl;
                for (const auto& enumInfo : analysis.parseResult.enums) {
                    std::cout << "     - Enum: " << enumInfo.name << std::endl;
                }
                
                std::cout << "   Includes found: " << analysis.parseResult.includes.size() << std::endl;
            } else {
                std::cout << "   Error: " << analysis.errorMessage << std::endl;
            }
        }
        
        // Export to JSON
        std::cout << "\n3. Exporting to JSON:" << std::endl;
        std::string jsonOutput = explorer.exportToJson();
        std::cout << "   JSON length: " << jsonOutput.length() << " bytes" << std::endl;
        
        // Save to file
        std::string outputFile = "source_analysis_output.json";
        if (explorer.exportToJsonFile(outputFile)) {
            std::cout << "   JSON saved to: " << outputFile << std::endl;
        } else {
            std::cout << "   Failed to save JSON file" << std::endl;
        }
        
        // Show a snippet of the JSON
        std::cout << "\n4. JSON snippet (first 500 characters):" << std::endl;
        std::string snippet = jsonOutput.substr(0, std::min(size_t(500), jsonOutput.length()));
        std::cout << snippet << "..." << std::endl;
        
    } else {
        std::cout << "   Error: " << result.errorMessage << std::endl;
    }
}

int main() {
    std::cout << "\n";
    printSeparator();
    std::cout << "UFM-TOOLING: FILE SYSTEM AND SOURCE EXPLORER DEMONSTRATION" << std::endl;
    printSeparator();

    try {
        // Demonstrate FileSystemExplorer
        demonstrateFileSystemExplorer();
        
        // Demonstrate SourceExplorer
        demonstrateSourceExplorer();

        std::cout << "\n";
        printSeparator();
        std::cout << "ALL DEMONSTRATIONS COMPLETED SUCCESSFULLY" << std::endl;
        printSeparator();
        std::cout << "\n";

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
