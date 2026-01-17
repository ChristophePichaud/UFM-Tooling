# UFM-Tooling API Documentation

## Table of Contents
1. [SimpleHeaderParser](#simpleheaderparser)
2. [PUMLClassParser](#pumlclassparser)
3. [PUMLEntityParser](#pumlentityparser)

---

## SimpleHeaderParser

### Purpose
Parse C++ header files to extract class information including members, methods, inheritance, and more.

### Header File
```cpp
#include "SimpleHeaderParser.h"
```

### Main Class

#### `SimpleHeaderParser`

**Constructor:**
```cpp
SimpleHeaderParser();
```

**Destructor:**
```cpp
~SimpleHeaderParser();
```

**Methods:**

##### `parseFile()`
```cpp
ParseResult parseFile(const std::string& filePath);
```
Parse a C++ header file from disk.
- **Parameters:**
  - `filePath`: Path to the header file
- **Returns:** `ParseResult` containing parsed information
- **Example:**
```cpp
SimpleHeaderParser parser;
ParseResult result = parser.parseFile("MyClass.h");
if (result.success) {
    // Use result
}
```

##### `parseContent()`
```cpp
ParseResult parseContent(const std::string& content, const std::string& fileName = "");
```
Parse C++ header content from a string.
- **Parameters:**
  - `content`: Header file content as string
  - `fileName`: Optional file name for reference
- **Returns:** `ParseResult` containing parsed information

##### `getClasses()`
```cpp
const std::vector<ClassInfo>& getClasses() const;
```
Get all classes found in the last parse.

##### `findClass()`
```cpp
const ClassInfo* findClass(const std::string& className) const;
```
Find a specific class by name.
- **Returns:** Pointer to ClassInfo or nullptr if not found

### Data Structures

#### `ClassInfo`
Represents a C++ class.

**Fields:**
- `std::string name` - Class name
- `std::string fullName` - Full name including namespace
- `std::vector<BaseClassInfo> baseClasses` - Base classes
- `std::vector<MemberInfo> members` - Member variables
- `std::vector<MethodInfo> methods` - Methods
- `bool isStruct` - True if this is a struct
- `bool isTemplate` - True if this is a template class

#### `MethodInfo`
Represents a method/function.

**Fields:**
- `std::string name` - Method name
- `std::string returnType` - Return type
- `AccessSpecifier access` - Access level (Public/Protected/Private)
- `std::vector<ParameterInfo> parameters` - Parameters
- `bool isStatic` - Static method
- `bool isVirtual` - Virtual method
- `bool isPureVirtual` - Pure virtual (= 0)
- `bool isConst` - Const method
- `bool isConstructor` - Constructor
- `bool isDestructor` - Destructor

#### `MemberInfo`
Represents a member variable.

**Fields:**
- `std::string name` - Member name
- `std::string type` - Member type
- `AccessSpecifier access` - Access level
- `bool isStatic` - Static member
- `bool isConst` - Const member
- `std::string defaultValue` - Default value if any

---

## PUMLClassParser

### Purpose
Parse PlantUML class diagrams to extract UML class information.

### Header File
```cpp
#include "PUMLClassParser.h"
```

### Main Class

#### `PUMLClassParser`

**Methods:**

##### `parseFile()`
```cpp
PUMLClassDiagramResult parseFile(const std::string& filePath);
```
Parse a PlantUML class diagram file.

##### `parseContent()`
```cpp
PUMLClassDiagramResult parseContent(const std::string& content);
```
Parse PlantUML content from a string.

##### `getClasses()`
```cpp
const std::vector<UMLClass>& getClasses() const;
```
Get all UML classes found.

##### `getRelationships()`
```cpp
const std::vector<UMLClassRelationship>& getRelationships() const;
```
Get all relationships found.

##### `findClass()`
```cpp
const UMLClass* findClass(const std::string& className) const;
```
Find a specific class by name.

##### `exportToJson()`
```cpp
std::string exportToJson() const;
```
Export parsed diagram to JSON format.

##### `exportToXML()`
```cpp
std::string exportToXML() const;
```
Export parsed diagram to XML format.

### Data Structures

#### `UMLClass`
Represents a UML class.

**Fields:**
- `std::string name` - Class name
- `std::string stereotype` - Stereotype (e.g., "entity", "controller")
- `std::vector<UMLAttribute> attributes` - Class attributes
- `std::vector<UMLMethod> methods` - Class methods
- `bool isAbstract` - Abstract class
- `bool isInterface` - Interface

#### `UMLAttribute`
Represents a UML attribute.

**Fields:**
- `std::string name` - Attribute name
- `std::string type` - Attribute type
- `UMLVisibility visibility` - Visibility (+, -, #, ~)
- `bool isStatic` - Static attribute
- `std::string defaultValue` - Default value

#### `UMLMethod`
Represents a UML method.

**Fields:**
- `std::string name` - Method name
- `std::string returnType` - Return type
- `UMLVisibility visibility` - Visibility
- `std::vector<UMLParameter> parameters` - Parameters
- `bool isStatic` - Static method
- `bool isAbstract` - Abstract method

#### `UMLClassRelationship`
Represents a relationship between classes.

**Fields:**
- `std::string fromClass` - Source class
- `std::string toClass` - Target class
- `UMLRelationship type` - Relationship type
  - `Association` (--)
  - `Inheritance` (--|>)
  - `Realization` (..|>)
  - `Composition` (*--)
  - `Aggregation` (o--)
  - `Dependency` (..>)
- `std::string label` - Relationship label
- `std::string fromCardinality` - Source cardinality
- `std::string toCardinality` - Target cardinality

### PlantUML Syntax Support

#### Visibility Modifiers
- `+` Public
- `-` Private
- `#` Protected
- `~` Package

#### Class Declaration
```plantuml
class ClassName {
    - attribute : type
    + method() : returnType
}

abstract class AbstractClass
interface InterfaceName
```

#### Relationships
```plantuml
ClassA --|> ClassB    ' Inheritance
ClassA ..|> InterfaceB ' Realization
ClassA *-- ClassB     ' Composition
ClassA o-- ClassB     ' Aggregation
ClassA --> ClassB     ' Association
ClassA ..> ClassB     ' Dependency
```

---

## PUMLEntityParser

### Purpose
Parse PlantUML entity/ER diagrams to extract database schema information.

### Header File
```cpp
#include "PUMLEntityParser.h"
```

### Main Class

#### `PUMLEntityParser`

**Methods:**

##### `parseFile()`
```cpp
PUMLEntityDiagramResult parseFile(const std::string& filePath);
```
Parse a PlantUML entity diagram file.

##### `parseContent()`
```cpp
PUMLEntityDiagramResult parseContent(const std::string& content);
```
Parse PlantUML entity diagram content from a string.

##### `getEntities()`
```cpp
const std::vector<Entity>& getEntities() const;
```
Get all entities found.

##### `getRelationships()`
```cpp
const std::vector<EntityRelationship>& getRelationships() const;
```
Get all relationships found.

##### `findEntity()`
```cpp
const Entity* findEntity(const std::string& entityName) const;
```
Find a specific entity by name.

##### `exportToJson()`
```cpp
std::string exportToJson() const;
```
Export to JSON format.

##### `exportToXML()`
```cpp
std::string exportToXML() const;
```
Export to XML format.

##### `exportToDDL()`
```cpp
std::string exportToDDL(const std::string& dialect = "SQL") const;
```
Generate SQL DDL (Data Definition Language) statements.
- **Parameters:**
  - `dialect`: SQL dialect (currently only "SQL" is supported)
- **Returns:** SQL CREATE TABLE statements

**Example:**
```cpp
PUMLEntityParser parser;
PUMLEntityDiagramResult result = parser.parseFile("schema.puml");
if (result.success) {
    std::string ddl = parser.exportToDDL("SQL");
    std::cout << ddl;
}
```

### Data Structures

#### `Entity`
Represents a database entity/table.

**Fields:**
- `std::string name` - Entity/table name
- `std::string alias` - Alias name
- `std::vector<EntityField> fields` - Fields/columns
- `std::string schema` - Database schema
- `std::string comment` - Entity comment

#### `EntityField`
Represents a field/column.

**Fields:**
- `std::string name` - Field name
- `std::string type` - Field type
- `bool isPrimaryKey` - Is primary key
- `bool isForeignKey` - Is foreign key
- `bool isUnique` - Has unique constraint
- `bool isNotNull` - Has NOT NULL constraint
- `std::string defaultValue` - Default value
- `std::string comment` - Field comment

#### `EntityRelationship`
Represents a relationship between entities.

**Fields:**
- `std::string fromEntity` - Source entity
- `std::string toEntity` - Target entity
- `Cardinality fromCardinality` - Source cardinality
- `Cardinality toCardinality` - Target cardinality
- `EntityRelationType type` - Relationship type
  - `OneToOne`
  - `OneToMany`
  - `ManyToOne`
  - `ManyToMany`
- `std::string label` - Relationship label
- `bool isIdentifying` - Identifying relationship

### PlantUML Entity Diagram Syntax

#### Field Markers
- `*` Primary key
- `+` Foreign key
- `#` Unique key
- `--` Field separator (visual only)

#### Entity Declaration
```plantuml
entity EntityName {
    * id : int <PK>
    --
    name : varchar(100)
    + foreign_id : int <FK>
}
```

#### Cardinality
- `||` Exactly one
- `|o` Zero or one
- `}|` One or many
- `}o` Zero or many

#### Relationships
```plantuml
Entity1 ||--o{ Entity2  ' One-to-many
Entity1 ||--|| Entity2  ' One-to-one
Entity1 }o--o{ Entity2  ' Many-to-many
```

---

## Common Enumerations

### AccessSpecifier
```cpp
enum class AccessSpecifier {
    Public,
    Protected,
    Private,
    None
};
```

### UMLVisibility
```cpp
enum class UMLVisibility {
    Public,      // +
    Private,     // -
    Protected,   // #
    Package      // ~
};
```

### Cardinality
```cpp
enum class Cardinality {
    ZeroOrOne,   // |o
    ExactlyOne,  // ||
    ZeroOrMany,  // }o
    OneOrMany    // }|
};
```

---

## Error Handling

All parsers return result structures with success indicators:

```cpp
if (result.success) {
    // Process result
} else {
    std::cerr << "Error: " << result.errorMessage << std::endl;
}
```

## Thread Safety

The parsers are **not thread-safe**. Create separate parser instances for each thread.

## Performance Considerations

- Parsers use in-memory processing
- File I/O is buffered
- Large files (>10MB) may take several seconds to parse
- Use `parseContent()` for already-loaded content to avoid file I/O

## Limitations

### SimpleHeaderParser
- Limited template support
- No preprocessor macro expansion
- Simplified expression parsing
- May not handle all C++ syntax variations

### PUMLClassParser
- Basic PlantUML syntax only
- Limited stereotype support
- Simplified note handling

### PUMLEntityParser
- Basic entity relationship syntax
- Limited constraint parsing
- DDL generation is simplified
- No support for indices, triggers, or stored procedures

## Examples Directory

See the `examples/` directory for:
- `example_usage.cpp` - Comprehensive examples of all parsers
- `test_samples.cpp` - Testing with sample files
- `sample_header.h` - Sample C++ header
- `sample_class_diagram.puml` - Sample class diagram
- `sample_entity_diagram.puml` - Sample entity diagram
