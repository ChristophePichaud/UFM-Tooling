# FileSystemExplorer and SourceExplorer API Documentation

## Overview

This document describes the `FileSystemExplorer` and `SourceExplorer` classes added to the UFM-Tooling library. These classes provide functionality for exploring file systems and analyzing C++ header files.

## FileSystemExplorer

### Purpose

`FileSystemExplorer` enumerates files and directories in a given base path, with support for both recursive and non-recursive exploration.

### Header File

```cpp
#include "FileSystemExplorer.h"
```

### Key Classes and Structures

#### FileSystemEntry

Represents a single file or directory entry.

```cpp
struct FileSystemEntry {
    std::string path;           // Full path to the file/directory
    std::string name;           // Name of the file/directory
    bool isDirectory;           // True if this is a directory
    size_t size;                // File size in bytes (0 for directories)
};
```

#### FileSystemExplorerResult

Contains the results of a file system exploration operation.

```cpp
struct FileSystemExplorerResult {
    std::vector<FileSystemEntry> entries;  // All found entries
    bool success;                          // True if exploration succeeded
    std::string errorMessage;              // Error message if failed
};
```

#### FileSystemExplorer Class

Main class for file system exploration.

```cpp
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
};
```

### Usage Example

```cpp
#include "FileSystemExplorer.h"
using namespace UFMTooling;

FileSystemExplorer explorer;

// Explore directory recursively
FileSystemExplorerResult result = explorer.explore("/path/to/code", true);

if (result.success) {
    std::cout << "Found " << result.entries.size() << " entries" << std::endl;
    
    // Get only header files
    std::vector<FileSystemEntry> headers = explorer.getFilesByExtension(".h");
    for (const auto& header : headers) {
        std::cout << "Header: " << header.path << std::endl;
    }
}
```

### Methods

#### explore()

```cpp
FileSystemExplorerResult explore(const std::string& basePath, bool bRecursive = true);
```

Explores a directory and returns all found files and directories.

**Parameters:**
- `basePath`: The directory path to explore
- `bRecursive`: If true, explores subdirectories recursively (default: true)

**Returns:** `FileSystemExplorerResult` containing all found entries

**Error Handling:** Returns result with `success = false` and appropriate error message if:
- Path does not exist
- Path is not a directory
- Permission errors or other filesystem exceptions occur

#### getFilesByExtension()

```cpp
std::vector<FileSystemEntry> getFilesByExtension(const std::string& extension) const;
```

Filters the last exploration result to return only files with the specified extension.

**Parameters:**
- `extension`: File extension to filter by (e.g., ".h", ".cpp", ".txt")

**Returns:** Vector of `FileSystemEntry` objects matching the extension

**Note:** Extension can be provided with or without the leading dot.

#### getDirectories()

```cpp
std::vector<FileSystemEntry> getDirectories() const;
```

Returns all directories from the last exploration result.

#### getFiles()

```cpp
std::vector<FileSystemEntry> getFiles() const;
```

Returns all files (non-directories) from the last exploration result.

---

## SourceExplorer

### Purpose

`SourceExplorer` discovers C++ header files (`.h`) in a directory structure, parses them using `SimpleHeaderParser`, and exports comprehensive analysis results to JSON format.

### Header File

```cpp
#include "SourceExplorer.h"
```

### Dependencies

- `SimpleHeaderParser` - For parsing C++ header files
- `FileSystemExplorer` - For discovering header files
- `nlohmann/json` - For JSON export functionality

### Key Classes and Structures

#### SourceFileAnalysis

Represents the analysis result of a single header file.

```cpp
struct SourceFileAnalysis {
    std::string path;           // Full path to the file
    std::string filename;       // Name of the file
    ParseResult parseResult;    // Result from SimpleHeaderParser
    bool success;               // True if parsing succeeded
    std::string errorMessage;   // Error message if failed
};
```

#### SourceExplorerResult

Contains the results of source code exploration.

```cpp
struct SourceExplorerResult {
    std::vector<SourceFileAnalysis> analyses;  // Analysis for each file
    bool success;                              // True if exploration succeeded
    std::string errorMessage;                  // Error message if failed
    int filesProcessed;                        // Number of files processed
    int filesWithErrors;                       // Number of files with parsing errors
};
```

#### SourceExplorer Class

Main class for exploring and analyzing source code.

```cpp
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
};
```

### Usage Example

```cpp
#include "SourceExplorer.h"
using namespace UFMTooling;

SourceExplorer explorer;

// Explore directory and analyze all .h files
SourceExplorerResult result = explorer.explore("src/include", true);

if (result.success) {
    std::cout << "Processed " << result.filesProcessed << " files" << std::endl;
    std::cout << "Errors: " << result.filesWithErrors << std::endl;
    
    // Access analysis for each file
    for (const auto& analysis : result.analyses) {
        if (analysis.success) {
            std::cout << "File: " << analysis.filename << std::endl;
            std::cout << "  Classes: " << analysis.parseResult.classes.size() << std::endl;
        }
    }
    
    // Export to JSON file
    if (explorer.exportToJsonFile("analysis.json")) {
        std::cout << "Analysis exported successfully" << std::endl;
    }
}
```

### Methods

#### explore()

```cpp
SourceExplorerResult explore(const std::string& basePath, bool bRecursive = true);
```

Explores a directory, finds all `.h` files, and parses each one.

**Parameters:**
- `basePath`: The directory path to explore
- `bRecursive`: If true, explores subdirectories recursively (default: true)

**Returns:** `SourceExplorerResult` containing analysis for all header files

**Process:**
1. Uses `FileSystemExplorer` to discover all `.h` files
2. Parses each header file using `SimpleHeaderParser`
3. Collects all parsing results
4. Returns comprehensive result with statistics

#### exportToJson()

```cpp
std::string exportToJson() const;
```

Exports the last exploration result to a JSON string.

**Returns:** Pretty-printed JSON string (2-space indentation)

**JSON Structure:**
```json
{
  "success": true,
  "filesProcessed": 5,
  "filesWithErrors": 0,
  "errorMessage": "",
  "files": [
    {
      "path": "path/to/file.h",
      "filename": "file.h",
      "success": true,
      "errorMessage": "",
      "classes": [
        {
          "name": "ClassName",
          "fullName": "Namespace::ClassName",
          "isStruct": false,
          "isTemplate": false,
          "baseClasses": [...],
          "members": [
            {
              "name": "memberName",
              "type": "int",
              "access": "private",
              "isStatic": false,
              "isConst": false,
              "defaultValue": ""
            }
          ],
          "methods": [
            {
              "name": "methodName",
              "returnType": "void",
              "access": "public",
              "isStatic": false,
              "isConst": false,
              "isVirtual": false,
              "isPureVirtual": false,
              "isConstructor": false,
              "isDestructor": false,
              "isOperator": false,
              "parameters": [...]
            }
          ],
          "templateParameters": [],
          "friendClasses": []
        }
      ],
      "enums": [...],
      "includes": [...]
    }
  ]
}
```

#### exportToJsonFile()

```cpp
bool exportToJsonFile(const std::string& filePath) const;
```

Exports the last exploration result to a JSON file.

**Parameters:**
- `filePath`: Path where the JSON file should be saved

**Returns:** `true` if file was written successfully, `false` otherwise

### JSON Export Details

The JSON export includes complete information for each analyzed file:

- **File Information**: path, filename, success status, error messages
- **Classes**: All parsed class information including:
  - Name and full name (with namespace)
  - Struct vs class indicator
  - Template information
  - Base classes with access specifiers
  - Member variables with types, access specifiers, and modifiers
  - Methods with return types, parameters, and modifiers (virtual, static, const, etc.)
  - Template parameters
  - Friend class declarations
- **Enums**: All enum definitions with values
- **Includes**: All include directives found in the file

### Error Handling

Both classes provide comprehensive error handling:

1. **Directory Access Errors**: If the base path doesn't exist or is not a directory, `explore()` returns a result with `success = false` and an appropriate error message.

2. **Permission Errors**: Files that cannot be accessed due to permissions are skipped silently to allow exploration to continue.

3. **Parsing Errors**: If a header file cannot be parsed, it is recorded in the results with `success = false` and the specific error message from the parser.

4. **File I/O Errors**: JSON export operations return `false` if file writing fails.

### Performance Considerations

- **Recursive Exploration**: Can be slow for very large directory trees. Use `bRecursive = false` for shallow exploration.
- **Large Files**: The `SimpleHeaderParser` reads entire files into memory. Very large header files may impact memory usage.
- **JSON Export**: The JSON string is constructed in memory. Exploring thousands of files may result in large JSON output.

### Thread Safety

Neither `FileSystemExplorer` nor `SourceExplorer` are thread-safe. If concurrent exploration is needed, create separate instances for each thread.

## Integration with Existing UFM-Tooling Classes

Both classes integrate seamlessly with existing UFM-Tooling functionality:

- `SourceExplorer` uses `SimpleHeaderParser` internally, producing `ParseResult` objects
- All structures from `SimpleHeaderParser` (ClassInfo, MethodInfo, MemberInfo, etc.) are preserved in the JSON export
- The same data structures can be used programmatically or exported to JSON

## Dependencies

### External Dependencies

- **C++17 Filesystem**: Required for directory traversal and file operations
- **nlohmann/json**: Single-header JSON library (included in `include/third_party/json.hpp`)

### Internal Dependencies

- `FileSystemExplorer` has no internal dependencies
- `SourceExplorer` depends on:
  - `FileSystemExplorer`
  - `SimpleHeaderParser`
  - `nlohmann/json`

## Building

Both classes are automatically included when building the UFM-Tooling library:

```bash
make clean
make library
```

The library file `libufmtooling.a` includes all necessary object files.

## Examples

Complete working examples are provided in:
- `examples/test_explorer.cpp` - Demonstrates both FileSystemExplorer and SourceExplorer

To build and run:
```bash
g++ -std=c++17 -Wall -Wextra -Iinclude examples/test_explorer.cpp -L. -lufmtooling -o bin/test_explorer
./bin/test_explorer
```
