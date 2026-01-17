#ifndef FILE_SYSTEM_EXPLORER_H
#define FILE_SYSTEM_EXPLORER_H

#include <string>
#include <vector>
#include <memory>

namespace UFMTooling {

    // Represents a file or directory in the file system
    struct FileSystemEntry {
        std::string path;           // Full path to the file/directory
        std::string name;           // Name of the file/directory
        bool isDirectory;           // True if this is a directory
        size_t size;                // File size in bytes (0 for directories)
        
        FileSystemEntry() : isDirectory(false), size(0) {}
    };

    // Result of file system exploration
    struct FileSystemExplorerResult {
        std::vector<FileSystemEntry> entries;
        bool success;
        std::string errorMessage;
        
        FileSystemExplorerResult() : success(false) {}
    };

    // Class for exploring file system structure
    class FileSystemExplorer {
    public:
        FileSystemExplorer();
        ~FileSystemExplorer();

        // Explore a directory with optional recursive scan
        FileSystemExplorerResult explore(const std::string& basePath, bool bRecursive = true);

        // Get all files with specific extension
        std::vector<FileSystemEntry> getFilesByExtension(const std::string& extension) const;

        // Get all directories
        std::vector<FileSystemEntry> getDirectories() const;

        // Get all files (non-directories)
        std::vector<FileSystemEntry> getFiles() const;

        // Get the last exploration result
        const FileSystemExplorerResult& getLastResult() const;

    private:
        class Impl;
        std::unique_ptr<Impl> pImpl;
    };

} // namespace UFMTooling

#endif // FILE_SYSTEM_EXPLORER_H
