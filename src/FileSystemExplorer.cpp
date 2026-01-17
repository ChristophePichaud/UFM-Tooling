#include "../include/FileSystemExplorer.h"
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

namespace UFMTooling {

    class FileSystemExplorer::Impl {
    public:
        FileSystemExplorerResult lastResult;

        FileSystemExplorerResult exploreDirectory(const std::string& basePath, bool bRecursive) {
            FileSystemExplorerResult result;
            
            try {
                fs::path base(basePath);
                
                // Check if the path exists
                if (!fs::exists(base)) {
                    result.errorMessage = "Path does not exist: " + basePath;
                    return result;
                }
                
                // Check if it's a directory
                if (!fs::is_directory(base)) {
                    result.errorMessage = "Path is not a directory: " + basePath;
                    return result;
                }
                
                // Iterate through the directory
                if (bRecursive) {
                    for (const auto& entry : fs::recursive_directory_iterator(base, fs::directory_options::skip_permission_denied)) {
                        try {
                            FileSystemEntry fsEntry;
                            fsEntry.path = entry.path().string();
                            fsEntry.name = entry.path().filename().string();
                            fsEntry.isDirectory = entry.is_directory();
                            
                            if (!fsEntry.isDirectory && entry.is_regular_file()) {
                                fsEntry.size = entry.file_size();
                            } else {
                                fsEntry.size = 0;
                            }
                            
                            result.entries.push_back(fsEntry);
                        } catch (const std::exception& e) {
                            // Skip entries that cause errors (permission denied, etc.)
                            continue;
                        }
                    }
                } else {
                    for (const auto& entry : fs::directory_iterator(base, fs::directory_options::skip_permission_denied)) {
                        try {
                            FileSystemEntry fsEntry;
                            fsEntry.path = entry.path().string();
                            fsEntry.name = entry.path().filename().string();
                            fsEntry.isDirectory = entry.is_directory();
                            
                            if (!fsEntry.isDirectory && entry.is_regular_file()) {
                                fsEntry.size = entry.file_size();
                            } else {
                                fsEntry.size = 0;
                            }
                            
                            result.entries.push_back(fsEntry);
                        } catch (const std::exception& e) {
                            // Skip entries that cause errors (permission denied, etc.)
                            continue;
                        }
                    }
                }
                
                result.success = true;
            } catch (const std::exception& e) {
                result.errorMessage = std::string("Error exploring directory: ") + e.what();
            }
            
            lastResult = result;
            return result;
        }

        std::vector<FileSystemEntry> filterByExtension(const std::string& extension) const {
            std::vector<FileSystemEntry> filtered;
            std::string ext = extension;
            
            // Ensure extension starts with a dot
            if (!ext.empty() && ext[0] != '.') {
                ext = "." + ext;
            }
            
            for (const auto& entry : lastResult.entries) {
                if (!entry.isDirectory) {
                    fs::path p(entry.path);
                    if (p.extension().string() == ext) {
                        filtered.push_back(entry);
                    }
                }
            }
            
            return filtered;
        }

        std::vector<FileSystemEntry> getDirectories() const {
            std::vector<FileSystemEntry> directories;
            for (const auto& entry : lastResult.entries) {
                if (entry.isDirectory) {
                    directories.push_back(entry);
                }
            }
            return directories;
        }

        std::vector<FileSystemEntry> getFiles() const {
            std::vector<FileSystemEntry> files;
            for (const auto& entry : lastResult.entries) {
                if (!entry.isDirectory) {
                    files.push_back(entry);
                }
            }
            return files;
        }
    };

    // FileSystemExplorer implementation
    FileSystemExplorer::FileSystemExplorer() : pImpl(new Impl()) {}

    FileSystemExplorer::~FileSystemExplorer() = default;

    FileSystemExplorerResult FileSystemExplorer::explore(const std::string& basePath, bool bRecursive) {
        return pImpl->exploreDirectory(basePath, bRecursive);
    }

    std::vector<FileSystemEntry> FileSystemExplorer::getFilesByExtension(const std::string& extension) const {
        return pImpl->filterByExtension(extension);
    }

    std::vector<FileSystemEntry> FileSystemExplorer::getDirectories() const {
        return pImpl->getDirectories();
    }

    std::vector<FileSystemEntry> FileSystemExplorer::getFiles() const {
        return pImpl->getFiles();
    }

    const FileSystemExplorerResult& FileSystemExplorer::getLastResult() const {
        return pImpl->lastResult;
    }

} // namespace UFMTooling
