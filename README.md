# UFM-Tooling

A comprehensive Win32 C++ library for parsing C++ header files and PlantUML diagrams.

## Overview

UFM-Tooling provides powerful parsers and explorers:

1. **SimpleHeaderParser** - Parse C++ header files and extract class structures, methods, members, and more
2. **PUMLClassParser** - Parse PlantUML class diagrams and extract UML class information
3. **PUMLEntityParser** - Parse PlantUML entity/ER diagrams and extract database schema information
4. **FileSystemExplorer** - Enumerate files and folders with recursive/non-recursive options
5. **SourceExplorer** - Explore directories for header files, parse them, and export to JSON
6. **ShapeElement & LayoutEngine** - Model and arrange UI elements with automatic layout algorithms

## Features

### SimpleHeaderParser
- Parse C++ header files (`.h`, `.hpp`)
- Extract class names, member variables, and methods
- Identify access specifiers (public, private, protected)
- Detect inheritance relationships
- Parse method parameters and return types
- Detect static, virtual, const, and pure virtual methods
- Parse struct and enum definitions
- Extract include directives

### PUMLClassParser
- Parse PlantUML class diagrams
- Extract classes, interfaces, and abstract classes
- Parse attributes and methods with visibility modifiers
- Identify relationships (inheritance, realization, composition, aggregation, etc.)
- Support for stereotypes and notes
- Export to JSON and XML formats

### PUMLEntityParser
- Parse PlantUML entity/ER diagrams
- Extract entities (tables) and their fields
- Identify primary keys, foreign keys, and constraints
- Parse relationships with cardinality
- Determine relationship types (one-to-one, one-to-many, many-to-many)
- Export to JSON, XML, and SQL DDL formats

### FileSystemExplorer
- Enumerate all files and folders in a directory
- Recursive or non-recursive exploration modes
- Filter files by extension
- Retrieve file metadata (size, type, path)
- Get all files or directories separately

### SourceExplorer
- Automatically discover and analyze C++ header files (`.h`)
- Recursively explore directory structures
- Parse each header file using SimpleHeaderParser
- Export comprehensive analysis to JSON format
- Includes all class information: members, methods, properties, inheritance
- Generate structured JSON reports with all parsing details

### ShapeElement & LayoutEngine
- Model drawing elements and relationships for UI applications
- Support for different element types (Drawing, Relationship)
- Automatic layout algorithms: Grid, Hierarchical, Force-Directed, Circular
- Configurable spacing, margins, and layout behavior
- Overlap detection and prevention
- Relationship-aware positioning for connected elements

## Building the Library

### Requirements
- Visual Studio 2019 or later
- Windows SDK 10.0 or later
- C++17 compiler

### Build Instructions

1. Open `UFM-Tooling.sln` in Visual Studio
2. Select your desired configuration (Debug/Release) and platform (x86/x64)
3. Build the solution (Ctrl+Shift+B)

The library will be compiled as a static library (`.lib` file).

## Usage

### SimpleHeaderParser Example

```cpp
#include "SimpleHeaderParser.h"
using namespace UFMTooling;

// Parse from file
SimpleHeaderParser parser;
ParseResult result = parser.parseFile("MyClass.h");

if (result.success) {
    // Iterate through classes
    for (const auto& cls : result.classes) {
        std::cout << "Class: " << cls.name << std::endl;
        
        // Access members
        for (const auto& member : cls.members) {
            std::cout << "  Member: " << member.type << " " << member.name << std::endl;
        }
        
        // Access methods
        for (const auto& method : cls.methods) {
            std::cout << "  Method: " << method.name << std::endl;
        }
    }
}

// Or parse from string
std::string headerContent = "class MyClass { public: int value; };";
ParseResult result2 = parser.parseContent(headerContent);
```

### PUMLClassParser Example

```cpp
#include "PUMLClassParser.h"
using namespace UFMTooling;

PUMLClassParser parser;
PUMLClassDiagramResult result = parser.parseFile("diagram.puml");

if (result.success) {
    // Iterate through classes
    for (const auto& cls : result.classes) {
        std::cout << "Class: " << cls.name << std::endl;
        
        // Access attributes
        for (const auto& attr : cls.attributes) {
            std::cout << "  Attribute: " << attr.name << std::endl;
        }
    }
    
    // Export to JSON
    std::string json = parser.exportToJson();
    std::cout << json << std::endl;
}
```

### PUMLEntityParser Example

```cpp
#include "PUMLEntityParser.h"
using namespace UFMTooling;

PUMLEntityParser parser;
PUMLEntityDiagramResult result = parser.parseFile("er_diagram.puml");

if (result.success) {
    // Iterate through entities
    for (const auto& entity : result.entities) {
        std::cout << "Entity: " << entity.name << std::endl;
        
        // Access fields
        for (const auto& field : entity.fields) {
            std::cout << "  Field: " << field.name << " : " << field.type;
            if (field.isPrimaryKey) std::cout << " (PK)";
            std::cout << std::endl;
        }
    }
    
    // Generate SQL DDL
    std::string ddl = parser.exportToDDL("SQL");
    std::cout << ddl << std::endl;
}
```

### FileSystemExplorer Example

```cpp
#include "FileSystemExplorer.h"
using namespace UFMTooling;

FileSystemExplorer explorer;

// Explore directory recursively
FileSystemExplorerResult result = explorer.explore("path/to/directory", true);

if (result.success) {
    std::cout << "Found " << result.entries.size() << " entries" << std::endl;
    
    // Get only header files
    std::vector<FileSystemEntry> headers = explorer.getFilesByExtension(".h");
    for (const auto& header : headers) {
        std::cout << "Header: " << header.path << std::endl;
    }
    
    // Get all directories
    std::vector<FileSystemEntry> dirs = explorer.getDirectories();
    
    // Get all files
    std::vector<FileSystemEntry> files = explorer.getFiles();
}
```

### SourceExplorer Example

```cpp
#include "SourceExplorer.h"
using namespace UFMTooling;

SourceExplorer explorer;

// Explore directory and analyze all .h files
SourceExplorerResult result = explorer.explore("src/include", true);

if (result.success) {
    std::cout << "Processed " << result.filesProcessed << " files" << std::endl;
    
    // Access analysis for each file
    for (const auto& analysis : result.analyses) {
        std::cout << "File: " << analysis.filename << std::endl;
        
        // Access parsed classes, methods, members
        for (const auto& cls : analysis.parseResult.classes) {
            std::cout << "  Class: " << cls.name << std::endl;
            std::cout << "    Members: " << cls.members.size() << std::endl;
            std::cout << "    Methods: " << cls.methods.size() << std::endl;
        }
    }
    
    // Export complete analysis to JSON
    std::string json = explorer.exportToJson();
    std::cout << json << std::endl;
    
    // Or save to file
    explorer.exportToJsonFile("analysis_output.json");
}
```

### LayoutEngine Example

```cpp
#include "ShapeElement.h"
#include "LayoutEngine.h"
using namespace UFMTooling;

// Create drawing elements
auto class1 = std::make_shared<DrawingElement>("UserClass");
class1->setSize(120.0, 80.0);

auto class2 = std::make_shared<DrawingElement>("OrderClass");
class2->setSize(120.0, 80.0);

auto class3 = std::make_shared<DrawingElement>("ProductClass");
class3->setSize(120.0, 80.0);

// Create relationships between elements
auto rel1 = std::make_shared<RelationshipElement>(class1, class2);
rel1->setRelationshipType("association");
rel1->setLabel("places");

auto rel2 = std::make_shared<RelationshipElement>(class2, class3);
rel2->setRelationshipType("contains");

// Collect all elements
std::vector<std::shared_ptr<ShapeElement>> elements;
elements.push_back(class1);
elements.push_back(class2);
elements.push_back(class3);
elements.push_back(rel1);
elements.push_back(rel2);

// Create layout engine with canvas size
CanvasSize canvas(1600.0, 900.0);
LayoutEngine engine(canvas);

// Configure layout
LayoutConfig config;
config.strategy = LayoutStrategy::Hierarchical;
config.padding = 30.0;
config.respectConnections = true;

// Arrange elements
LayoutResult result = engine.arrangeElements(elements, config);

if (result.success) {
    std::cout << "Arranged " << result.elementsArranged << " elements" << std::endl;
    
    // Access positioned elements
    for (const auto& elem : elements) {
        if (elem->getElementType() == ElementType::Drawing) {
            auto drawing = std::static_pointer_cast<DrawingElement>(elem);
            std::cout << drawing->getName() 
                     << " at (" << elem->getPosition().x 
                     << ", " << elem->getPosition().y << ")" << std::endl;
        }
    }
}
```

## Data Structures

### SimpleHeaderParser

Key structures:
- `ClassInfo` - Represents a C++ class
- `MethodInfo` - Represents a method/function
- `MemberInfo` - Represents a member variable
- `ParameterInfo` - Represents a method parameter
- `ParseResult` - Contains parsing results

### PUMLClassParser

Key structures:
- `UMLClass` - Represents a UML class
- `UMLMethod` - Represents a UML method
- `UMLAttribute` - Represents a UML attribute
- `UMLClassRelationship` - Represents a relationship between classes
- `PUMLClassDiagramResult` - Contains parsing results

### PUMLEntityParser

Key structures:
- `Entity` - Represents a database entity/table
- `EntityField` - Represents a field/column
- `EntityRelationship` - Represents a relationship between entities
- `PUMLEntityDiagramResult` - Contains parsing results

### FileSystemExplorer

Key structures:
- `FileSystemEntry` - Represents a file or directory with path, name, size, and type
- `FileSystemExplorerResult` - Contains exploration results with all found entries

### SourceExplorer

Key structures:
- `SourceFileAnalysis` - Analysis result for a single header file
- `SourceExplorerResult` - Contains all file analyses with statistics

### ShapeElement & LayoutEngine

Key structures:
- `ShapeElement` - Base class for all shape elements
- `DrawingElement` - Represents a drawing element with name, shape type, and color
- `RelationshipElement` - Represents a relationship between two drawing elements
- `Position` - 2D position (x, y coordinates)
- `Size` - Dimensions (width, height)
- `LayoutEngine` - Arranges elements using various layout strategies
- `LayoutConfig` - Configuration for layout behavior
- `LayoutResult` - Results of layout operation
- `CanvasSize` - Canvas dimensions

Layout strategies:
- `Grid` - Arrange elements in a uniform grid pattern
- `Hierarchical` - Arrange elements in levels based on relationships
- `Force` - Use physics-based force-directed layout
- `Circular` - Arrange elements in a circular pattern

## Dependencies

- **C++17 or later** - Required for filesystem support
- **nlohmann/json** - Included as single-header library for JSON export functionality

## Example

Complete examples are available:
- `examples/example_usage.cpp` - Demonstrates SimpleHeaderParser, PUMLClassParser, and PUMLEntityParser
- `examples/test_explorer.cpp` - Demonstrates FileSystemExplorer and SourceExplorer with JSON export
- `examples/layout_example.cpp` - Demonstrates ShapeElement and LayoutEngine with various layout strategies

## Architecture

The library uses the Pimpl (Pointer to Implementation) idiom to hide implementation details and reduce compile-time dependencies. Each parser has:
- A public header file with the API (`include/*.h`)
- An implementation file with the parsing logic (`src/*.cpp`)
- An internal `Impl` class that handles the actual parsing

## Limitations

### SimpleHeaderParser
- Limited support for template classes
- Does not handle preprocessor macros
- Simplified parsing of complex expressions
- May not handle all C++ syntax variations

### PUMLClassParser
- Focuses on basic PlantUML class diagram syntax
- Limited support for advanced PlantUML features
- Notes are simplified

### PUMLEntityParser
- Focuses on basic entity relationship syntax
- Limited support for advanced ER diagram features
- DDL generation is basic and may need customization

## License

This project is open source. See LICENSE file for details.

## Contributing

Contributions are welcome! Please submit pull requests or open issues for bugs and feature requests.

## Author

Christophe Pichaud