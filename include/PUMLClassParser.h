#ifndef PUML_CLASS_PARSER_H
#define PUML_CLASS_PARSER_H

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace UFMTooling {

    // Enum for visibility in UML
    enum class UMLVisibility {
        Public,      // +
        Private,     // -
        Protected,   // #
        Package      // ~
    };

    // Enum for relationship types
    enum class UMLRelationship {
        Association,        // --
        Dependency,         // ..>
        Aggregation,        // o--
        Composition,        // *--
        Inheritance,        // --|>
        Realization,        // ..|>
        DirectedAssociation // -->
    };

    // Represents a UML attribute
    struct UMLAttribute {
        std::string name;
        std::string type;
        UMLVisibility visibility;
        bool isStatic;
        std::string defaultValue;
        std::string stereotype;

        UMLAttribute() : visibility(UMLVisibility::Private), isStatic(false) {}
    };

    // Represents a UML method parameter
    struct UMLParameter {
        std::string name;
        std::string type;
        std::string direction; // in, out, inout
        std::string defaultValue;

        UMLParameter() : direction("in") {}
    };

    // Represents a UML method
    struct UMLMethod {
        std::string name;
        std::string returnType;
        UMLVisibility visibility;
        std::vector<UMLParameter> parameters;
        bool isStatic;
        bool isAbstract;
        std::string stereotype;

        UMLMethod() : visibility(UMLVisibility::Public), isStatic(false), isAbstract(false) {}
    };

    // Represents a UML class
    struct UMLClass {
        std::string name;
        std::string stereotype;
        std::vector<UMLAttribute> attributes;
        std::vector<UMLMethod> methods;
        bool isAbstract;
        bool isInterface;
        std::string package;
        std::string note;

        UMLClass() : isAbstract(false), isInterface(false) {}
    };

    // Represents a relationship between classes
    struct UMLClassRelationship {
        std::string fromClass;
        std::string toClass;
        UMLRelationship type;
        std::string label;
        std::string fromCardinality;
        std::string toCardinality;

        UMLClassRelationship() : type(UMLRelationship::Association) {}
    };

    // Main parse result for class diagrams
    struct PUMLClassDiagramResult {
        std::string title;
        std::vector<UMLClass> classes;
        std::vector<UMLClassRelationship> relationships;
        std::map<std::string, std::string> notes;
        bool success;
        std::string errorMessage;

        PUMLClassDiagramResult() : success(false) {}
    };

    // PUML Class Diagram Parser
    class PUMLClassParser {
    public:
        PUMLClassParser();
        ~PUMLClassParser();

        // Parse a PlantUML file
        PUMLClassDiagramResult parseFile(const std::string& filePath);

        // Parse PlantUML content from string
        PUMLClassDiagramResult parseContent(const std::string& content);

        // Get all classes found
        const std::vector<UMLClass>& getClasses() const;

        // Get all relationships found
        const std::vector<UMLClassRelationship>& getRelationships() const;

        // Find a class by name
        const UMLClass* findClass(const std::string& className) const;

        // Export to different formats
        std::string exportToJson() const;
        std::string exportToXML() const;

        // Get parsing warnings
        const std::vector<std::string>& getWarnings() const;

    private:
        class Impl;
        std::unique_ptr<Impl> pImpl;
    };

} // namespace UFMTooling

#endif // PUML_CLASS_PARSER_H
