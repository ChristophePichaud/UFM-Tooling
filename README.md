# UFM-Tooling

A comprehensive Win32 C++ library for parsing C++ header files and PlantUML diagrams.

## Overview

UFM-Tooling provides three powerful parsers:

1. **SimpleHeaderParser** - Parse C++ header files and extract class structures, methods, members, and more
2. **PUMLClassParser** - Parse PlantUML class diagrams and extract UML class information
3. **PUMLEntityParser** - Parse PlantUML entity/ER diagrams and extract database schema information

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

## Example

A complete example demonstrating all three parsers is available in `examples/example_usage.cpp`. This example shows:
- How to parse C++ headers with classes, inheritance, and methods
- How to parse PlantUML class diagrams with relationships
- How to parse PlantUML entity diagrams and generate SQL DDL

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