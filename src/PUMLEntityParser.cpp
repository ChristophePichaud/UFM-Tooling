#include "../include/PUMLEntityParser.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>

namespace UFMTooling {

    // Helper functions
    namespace {
        std::string trim(const std::string& str) {
            size_t first = str.find_first_not_of(" \t\n\r");
            if (first == std::string::npos) return "";
            size_t last = str.find_last_not_of(" \t\n\r");
            return str.substr(first, last - first + 1);
        }

        Cardinality parseCardinality(const std::string& card) {
            if (card.find("|o") != std::string::npos) return Cardinality::ZeroOrOne;
            if (card.find("||") != std::string::npos) return Cardinality::ExactlyOne;
            if (card.find("}o") != std::string::npos) return Cardinality::ZeroOrMany;
            if (card.find("}|") != std::string::npos) return Cardinality::OneOrMany;
            return Cardinality::ExactlyOne;
        }

        EntityRelationType determineRelationType(Cardinality from, Cardinality to) {
            bool fromMany = (from == Cardinality::ZeroOrMany || from == Cardinality::OneOrMany);
            bool toMany = (to == Cardinality::ZeroOrMany || to == Cardinality::OneOrMany);

            if (fromMany && toMany) return EntityRelationType::ManyToMany;
            if (fromMany && !toMany) return EntityRelationType::ManyToOne;
            if (!fromMany && toMany) return EntityRelationType::OneToMany;
            return EntityRelationType::OneToOne;
        }
    }

    // Implementation class
    class PUMLEntityParser::Impl {
    public:
        std::vector<Entity> entities;
        std::vector<EntityRelationship> relationships;
        std::map<std::string, std::string> notes;
        std::vector<std::string> warnings;
        std::string title;

        PUMLEntityDiagramResult parse(const std::string& content) {
            PUMLEntityDiagramResult result;
            result.success = true;

            entities.clear();
            relationships.clear();
            notes.clear();
            warnings.clear();

            try {
                std::istringstream stream(content);
                std::string line;
                bool inEntity = false;
                Entity currentEntity;
                bool inPlantUML = false;

                while (std::getline(stream, line)) {
                    line = trim(line);
                    
                    // Skip empty lines and comments
                    if (line.empty() || line[0] == '\'') continue;

                    // Check for PlantUML start/end
                    if (line.find("@startuml") != std::string::npos) {
                        inPlantUML = true;
                        continue;
                    }
                    if (line.find("@enduml") != std::string::npos) {
                        inPlantUML = false;
                        if (inEntity) {
                            entities.push_back(currentEntity);
                            inEntity = false;
                        }
                        continue;
                    }

                    if (!inPlantUML) continue;

                    // Parse title
                    if (line.find("title ") == 0) {
                        title = line.substr(6);
                        result.title = title;
                        continue;
                    }

                    // Parse entity declaration
                    if (line.find("entity ") != std::string::npos || 
                        line.find("table ") != std::string::npos) {
                        
                        if (inEntity) {
                            entities.push_back(currentEntity);
                        }

                        currentEntity = Entity();
                        inEntity = true;

                        // Extract entity name
                        size_t entityPos = line.find("entity ");
                        if (entityPos == std::string::npos) {
                            entityPos = line.find("table ");
                        }
                        
                        if (entityPos != std::string::npos) {
                            std::string rest = line.substr(entityPos);
                            size_t nameStart = rest.find(" ") + 1;
                            std::string nameStr = trim(rest.substr(nameStart));
                            
                            // Handle "Entity as Alias" syntax
                            size_t asPos = nameStr.find(" as ");
                            if (asPos != std::string::npos) {
                                currentEntity.name = trim(nameStr.substr(0, asPos));
                                std::string aliasStr = trim(nameStr.substr(asPos + 4));
                                size_t aliasEnd = aliasStr.find_first_of(" {");
                                if (aliasEnd != std::string::npos) {
                                    currentEntity.alias = aliasStr.substr(0, aliasEnd);
                                } else {
                                    currentEntity.alias = aliasStr;
                                }
                            } else {
                                size_t nameEnd = nameStr.find_first_of(" {");
                                if (nameEnd != std::string::npos) {
                                    currentEntity.name = nameStr.substr(0, nameEnd);
                                } else {
                                    currentEntity.name = nameStr;
                                }
                            }
                        }
                        continue;
                    }

                    // End of entity
                    if (line == "}" && inEntity) {
                        entities.push_back(currentEntity);
                        inEntity = false;
                        currentEntity = Entity();
                        continue;
                    }

                    // Parse entity fields (inside entity)
                    if (inEntity && line != "{") {
                        parseField(line, currentEntity);
                        continue;
                    }

                    // Parse relationships
                    if (line.find("--") != std::string::npos || 
                        line.find("..") != std::string::npos ||
                        line.find("|") != std::string::npos ||
                        line.find("}") != std::string::npos) {
                        parseRelationshipLine(line);
                    }

                    // Parse notes
                    if (line.find("note") != std::string::npos) {
                        parseNote(line);
                    }
                }

                // Save last entity if needed
                if (inEntity) {
                    entities.push_back(currentEntity);
                }

                result.entities = entities;
                result.relationships = relationships;
                result.notes = notes;

            } catch (const std::exception& e) {
                result.success = false;
                result.errorMessage = std::string("Parsing error: ") + e.what();
            }

            return result;
        }

    private:
        void parseField(const std::string& line, Entity& entity) {
            EntityField field;
            std::string content = trim(line);

            // Skip separator lines
            if (content == "--" || content.empty()) {
                return;
            }

            // Check for primary key marker (*)
            if (content.find("*") == 0) {
                field.isPrimaryKey = true;
                content = trim(content.substr(1));
            }

            // Check for foreign key marker (+)
            if (content.find("+") == 0) {
                field.isForeignKey = true;
                content = trim(content.substr(1));
            }

            // Check for unique marker (#)
            if (content.find("#") == 0) {
                field.isUnique = true;
                content = trim(content.substr(1));
            }

            // Parse field name : type
            size_t colonPos = content.find(":");
            if (colonPos != std::string::npos) {
                field.name = trim(content.substr(0, colonPos));
                std::string typeStr = trim(content.substr(colonPos + 1));
                
                // Check for constraints in angle brackets
                size_t bracketPos = typeStr.find("<");
                if (bracketPos != std::string::npos) {
                    field.type = trim(typeStr.substr(0, bracketPos));
                    size_t closeBracket = typeStr.find(">", bracketPos);
                    if (closeBracket != std::string::npos) {
                        std::string constraints = typeStr.substr(bracketPos + 1, closeBracket - bracketPos - 1);
                        parseConstraints(constraints, field);
                    }
                } else {
                    field.type = typeStr;
                }
            } else {
                // Just field name
                field.name = content;
            }

            // Set constraint flags
            if (field.isPrimaryKey) {
                field.constraints.push_back(EntityFieldType::PrimaryKey);
                field.isNotNull = true;
            }
            if (field.isForeignKey) {
                field.constraints.push_back(EntityFieldType::ForeignKey);
            }
            if (field.isUnique) {
                field.constraints.push_back(EntityFieldType::Unique);
            }
            if (field.isNotNull) {
                field.constraints.push_back(EntityFieldType::NotNull);
            }

            entity.fields.push_back(field);
        }

        void parseConstraints(const std::string& constraints, EntityField& field) {
            std::istringstream stream(constraints);
            std::string constraint;
            while (std::getline(stream, constraint, ',')) {
                constraint = trim(constraint);
                if (constraint == "PK" || constraint == "pk") {
                    field.isPrimaryKey = true;
                } else if (constraint == "FK" || constraint == "fk") {
                    field.isForeignKey = true;
                } else if (constraint == "UK" || constraint == "unique") {
                    field.isUnique = true;
                } else if (constraint == "NOT NULL" || constraint == "notnull") {
                    field.isNotNull = true;
                }
            }
        }

        void parseRelationshipLine(const std::string& line) {
            EntityRelationship rel;
            
            // Look for relationship patterns like: Entity1 ||--o{ Entity2
            // Pattern: [Entity] [leftCard] -- [rightCard] [Entity]
            
            std::regex relRegex(R"((\w+)\s*(\|\||\|o|\}o|\}\|)\s*-+\s*(\|\||\|o|\}o|\}\|)\s*(\w+))");
            std::smatch match;
            
            if (std::regex_search(line, match, relRegex)) {
                rel.fromEntity = match[1].str();
                rel.fromCardinality = parseCardinality(match[2].str());
                rel.toCardinality = parseCardinality(match[3].str());
                rel.toEntity = match[4].str();
                rel.type = determineRelationType(rel.fromCardinality, rel.toCardinality);
                
                // Look for label
                size_t colonPos = line.find(":");
                if (colonPos != std::string::npos) {
                    rel.label = trim(line.substr(colonPos + 1));
                }
                
                relationships.push_back(rel);
            } else {
                // Try simpler pattern: Entity1 -- Entity2
                std::regex simpleRegex(R"((\w+)\s*-+\s*(\w+))");
                if (std::regex_search(line, match, simpleRegex)) {
                    rel.fromEntity = match[1].str();
                    rel.toEntity = match[2].str();
                    rel.type = EntityRelationType::OneToMany;
                    relationships.push_back(rel);
                }
            }
        }

        void parseNote(const std::string& line) {
            // Simple note parsing
            if (line.find("note") != std::string::npos) {
                size_t ofPos = line.find(" of ");
                if (ofPos != std::string::npos) {
                    std::string entityName = trim(line.substr(ofPos + 4));
                    notes[entityName] = "Note"; // Simplified
                }
            }
        }
    };

    // PUMLEntityParser implementation
    PUMLEntityParser::PUMLEntityParser() : pImpl(new Impl()) {}

    PUMLEntityParser::~PUMLEntityParser() = default;

    PUMLEntityDiagramResult PUMLEntityParser::parseFile(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            PUMLEntityDiagramResult result;
            result.success = false;
            result.errorMessage = "Could not open file: " + filePath;
            return result;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        return parseContent(buffer.str());
    }

    PUMLEntityDiagramResult PUMLEntityParser::parseContent(const std::string& content) {
        return pImpl->parse(content);
    }

    const std::vector<Entity>& PUMLEntityParser::getEntities() const {
        return pImpl->entities;
    }

    const std::vector<EntityRelationship>& PUMLEntityParser::getRelationships() const {
        return pImpl->relationships;
    }

    const Entity* PUMLEntityParser::findEntity(const std::string& entityName) const {
        for (const auto& entity : pImpl->entities) {
            if (entity.name == entityName || entity.alias == entityName) {
                return &entity;
            }
        }
        return nullptr;
    }

    std::string PUMLEntityParser::exportToJson() const {
        // Basic JSON export
        std::stringstream json;
        json << "{\n";
        json << "  \"entities\": [\n";
        
        for (size_t i = 0; i < pImpl->entities.size(); ++i) {
            const auto& entity = pImpl->entities[i];
            json << "    {\n";
            json << "      \"name\": \"" << entity.name << "\",\n";
            json << "      \"fields\": [\n";
            
            for (size_t j = 0; j < entity.fields.size(); ++j) {
                const auto& field = entity.fields[j];
                json << "        {\n";
                json << "          \"name\": \"" << field.name << "\",\n";
                json << "          \"type\": \"" << field.type << "\",\n";
                json << "          \"isPrimaryKey\": " << (field.isPrimaryKey ? "true" : "false") << "\n";
                json << "        }";
                if (j < entity.fields.size() - 1) json << ",";
                json << "\n";
            }
            
            json << "      ]\n";
            json << "    }";
            if (i < pImpl->entities.size() - 1) json << ",";
            json << "\n";
        }
        
        json << "  ]\n";
        json << "}\n";
        return json.str();
    }

    std::string PUMLEntityParser::exportToXML() const {
        // Basic XML export
        std::stringstream xml;
        xml << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        xml << "<EntityDiagram>\n";
        
        for (const auto& entity : pImpl->entities) {
            xml << "  <Entity name=\"" << entity.name << "\">\n";
            for (const auto& field : entity.fields) {
                xml << "    <Field name=\"" << field.name << "\" ";
                xml << "type=\"" << field.type << "\" ";
                xml << "isPrimaryKey=\"" << (field.isPrimaryKey ? "true" : "false") << "\" />\n";
            }
            xml << "  </Entity>\n";
        }
        
        xml << "</EntityDiagram>\n";
        return xml.str();
    }

    std::string PUMLEntityParser::exportToDDL(const std::string& dialect) const {
        // Generate SQL DDL
        (void)dialect; // Suppress unused parameter warning
        std::stringstream ddl;
        
        for (const auto& entity : pImpl->entities) {
            ddl << "CREATE TABLE " << entity.name << " (\n";
            
            for (size_t i = 0; i < entity.fields.size(); ++i) {
                const auto& field = entity.fields[i];
                ddl << "    " << field.name << " " << field.type;
                
                if (field.isPrimaryKey) {
                    ddl << " PRIMARY KEY";
                }
                if (field.isNotNull && !field.isPrimaryKey) {
                    ddl << " NOT NULL";
                }
                if (field.isUnique && !field.isPrimaryKey) {
                    ddl << " UNIQUE";
                }
                
                if (i < entity.fields.size() - 1) {
                    ddl << ",";
                }
                ddl << "\n";
            }
            
            ddl << ");\n\n";
        }
        
        return ddl.str();
    }

    const std::vector<std::string>& PUMLEntityParser::getWarnings() const {
        return pImpl->warnings;
    }

} // namespace UFMTooling
