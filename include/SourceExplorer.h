#ifndef SOURCE_EXPLORER_H
#define SOURCE_EXPLORER_H

#include "SimpleHeaderParser.h"
#include <string>
#include <vector>
#include <memory>

namespace UFMTooling {

    // Forward declarations
    struct FileSystemEntry;

    // Represents the result of analyzing a single header file
    struct SourceFileAnalysis {
        std::string path;           // Full path to the file
        std::string filename;       // Name of the file
        ParseResult parseResult;    // Result from SimpleHeaderParser
        bool success;
        std::string errorMessage;
        
        SourceFileAnalysis() : success(false) {}
    };

    // Result of source code exploration
    struct SourceExplorerResult {
        std::vector<SourceFileAnalysis> analyses;
        bool success;
        std::string errorMessage;
        int filesProcessed;
        int filesWithErrors;
        
        SourceExplorerResult() : success(false), filesProcessed(0), filesWithErrors(0) {}
    };

    // Class for exploring source code and analyzing header files
    class SourceExplorer {
    public:
        SourceExplorer();
        ~SourceExplorer();

        // Explore a directory and analyze all .h files
        SourceExplorerResult explore(const std::string& basePath, bool bRecursive = true);

        // Export the last exploration result to JSON
        std::string exportToJson() const;

        // Export the last exploration result to a JSON file
        bool exportToJsonFile(const std::string& filePath) const;

        // Get the last exploration result
        const SourceExplorerResult& getLastResult() const;

    private:
        class Impl;
        std::unique_ptr<Impl> pImpl;
    };

} // namespace UFMTooling

#endif // SOURCE_EXPLORER_H
