#ifndef PUML_ENTITY_PARSER_H
#define PUML_ENTITY_PARSER_H

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace UFMTooling {

    // Enum for field types in entity diagrams
    enum class EntityFieldType {
        PrimaryKey,
        ForeignKey,
        Unique,
        NotNull,
        Regular
    };

    // Represents a field in an entity
    struct EntityField {
        std::string name;
        std::string type;
        std::vector<EntityFieldType> constraints;
        std::string defaultValue;
        std::string comment;
        bool isPrimaryKey;
        bool isForeignKey;
        bool isUnique;
        bool isNotNull;

        EntityField() : isPrimaryKey(false), isForeignKey(false), 
                       isUnique(false), isNotNull(false) {}
    };

    // Represents an entity (table) in ER diagram
    struct Entity {
        std::string name;
        std::string alias;
        std::vector<EntityField> fields;
        std::string schema;
        std::string comment;
        std::string stereotype;

        Entity() {}
    };

    // Enum for relationship cardinality
    enum class Cardinality {
        ZeroOrOne,      // |o
        ExactlyOne,     // ||
        ZeroOrMany,     // }o
        OneOrMany       // }|
    };

    // Enum for relationship type
    enum class EntityRelationType {
        OneToOne,
        OneToMany,
        ManyToOne,
        ManyToMany
    };

    // Represents a relationship between entities
    struct EntityRelationship {
        std::string fromEntity;
        std::string toEntity;
        Cardinality fromCardinality;
        Cardinality toCardinality;
        EntityRelationType type;
        std::string label;
        std::vector<std::string> fromFields;
        std::vector<std::string> toFields;
        bool isIdentifying;

        EntityRelationship() : fromCardinality(Cardinality::ExactlyOne),
                               toCardinality(Cardinality::ZeroOrMany),
                               type(EntityRelationType::OneToMany),
                               isIdentifying(false) {}
    };

    // Main parse result for entity diagrams
    struct PUMLEntityDiagramResult {
        std::string title;
        std::vector<Entity> entities;
        std::vector<EntityRelationship> relationships;
        std::map<std::string, std::string> notes;
        bool success;
        std::string errorMessage;

        PUMLEntityDiagramResult() : success(false) {}
    };

    // PUML Entity/ER Diagram Parser
    class PUMLEntityParser {
    public:
        PUMLEntityParser();
        ~PUMLEntityParser();

        // Parse a PlantUML entity diagram file
        PUMLEntityDiagramResult parseFile(const std::string& filePath);

        // Parse PlantUML entity diagram content from string
        PUMLEntityDiagramResult parseContent(const std::string& content);

        // Get all entities found
        const std::vector<Entity>& getEntities() const;

        // Get all relationships found
        const std::vector<EntityRelationship>& getRelationships() const;

        // Find an entity by name
        const Entity* findEntity(const std::string& entityName) const;

        // Export to different formats
        std::string exportToJson() const;
        std::string exportToXML() const;
        std::string exportToDDL(const std::string& dialect = "SQL") const; // Generate SQL DDL

        // Get parsing warnings
        const std::vector<std::string>& getWarnings() const;

    private:
        class Impl;
        std::unique_ptr<Impl> pImpl;
    };

} // namespace UFMTooling

#endif // PUML_ENTITY_PARSER_H
