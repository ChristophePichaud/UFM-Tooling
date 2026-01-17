#include "../include/PUMLClassParser.h"
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

        UMLVisibility parseVisibility(char symbol) {
            switch (symbol) {
                case '+': return UMLVisibility::Public;
                case '-': return UMLVisibility::Private;
                case '#': return UMLVisibility::Protected;
                case '~': return UMLVisibility::Package;
                default: return UMLVisibility::Private;
            }
        }

        UMLRelationship parseRelationship(const std::string& rel) {
            std::string r = trim(rel);
            if (r.find("--|>") != std::string::npos) return UMLRelationship::Inheritance;
            if (r.find("..|>") != std::string::npos) return UMLRelationship::Realization;
            if (r.find("*--") != std::string::npos) return UMLRelationship::Composition;
            if (r.find("o--") != std::string::npos) return UMLRelationship::Aggregation;
            if (r.find("-->") != std::string::npos) return UMLRelationship::DirectedAssociation;
            if (r.find("..>") != std::string::npos) return UMLRelationship::Dependency;
            return UMLRelationship::Association;
        }
    }

    // Implementation class
    class PUMLClassParser::Impl {
    public:
        std::vector<UMLClass> classes;
        std::vector<UMLClassRelationship> relationships;
        std::map<std::string, std::string> notes;
        std::vector<std::string> warnings;
        std::string title;

        PUMLClassDiagramResult parse(const std::string& content) {
            PUMLClassDiagramResult result;
            result.success = true;

            classes.clear();
            relationships.clear();
            notes.clear();
            warnings.clear();

            try {
                std::istringstream stream(content);
                std::string line;
                bool inClass = false;
                UMLClass currentClass;
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
                        if (inClass) {
                            classes.push_back(currentClass);
                            inClass = false;
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

                    // Parse class declaration
                    if (line.find("class ") != std::string::npos || 
                        line.find("interface ") != std::string::npos ||
                        line.find("abstract ") != std::string::npos) {
                        
                        if (inClass) {
                            classes.push_back(currentClass);
                        }

                        currentClass = UMLClass();
                        inClass = true;

                        // Check for abstract or interface
                        currentClass.isAbstract = line.find("abstract") != std::string::npos;
                        currentClass.isInterface = line.find("interface") != std::string::npos;

                        // Extract class name
                        size_t classPos = line.find("class ");
                        if (classPos == std::string::npos) {
                            classPos = line.find("interface ");
                        }
                        
                        if (classPos != std::string::npos) {
                            std::string rest = line.substr(classPos);
                            size_t nameStart = rest.find(" ") + 1;
                            std::string nameStr = trim(rest.substr(nameStart));
                            
                            // Check for stereotype
                            if (nameStr.find("<<") != std::string::npos) {
                                size_t stereoStart = nameStr.find("<<");
                                size_t stereoEnd = nameStr.find(">>");
                                if (stereoEnd != std::string::npos) {
                                    currentClass.stereotype = nameStr.substr(stereoStart + 2, stereoEnd - stereoStart - 2);
                                    nameStr = trim(nameStr.substr(0, stereoStart)) + trim(nameStr.substr(stereoEnd + 2));
                                }
                            }

                            size_t nameEnd = nameStr.find_first_of(" {");
                            if (nameEnd != std::string::npos) {
                                currentClass.name = nameStr.substr(0, nameEnd);
                            } else {
                                currentClass.name = nameStr;
                            }
                        }
                        continue;
                    }

                    // End of class
                    if (line == "}" && inClass) {
                        classes.push_back(currentClass);
                        inClass = false;
                        currentClass = UMLClass();
                        continue;
                    }

                    // Parse class members (inside class)
                    if (inClass && line != "{") {
                        if (line[0] == '+' || line[0] == '-' || line[0] == '#' || line[0] == '~') {
                            // Parse attribute or method
                            if (line.find("(") != std::string::npos) {
                                parseMethod(line, currentClass);
                            } else {
                                parseAttribute(line, currentClass);
                            }
                        }
                        continue;
                    }

                    // Parse relationships
                    if (line.find("--") != std::string::npos || 
                        line.find("..") != std::string::npos ||
                        line.find("<|") != std::string::npos ||
                        line.find("|>") != std::string::npos) {
                        parseRelationshipLine(line);
                    }

                    // Parse notes
                    if (line.find("note") != std::string::npos) {
                        parseNote(line);
                    }
                }

                // Save last class if needed
                if (inClass) {
                    classes.push_back(currentClass);
                }

                result.classes = classes;
                result.relationships = relationships;
                result.notes = notes;

            } catch (const std::exception& e) {
                result.success = false;
                result.errorMessage = std::string("Parsing error: ") + e.what();
            }

            return result;
        }

    private:
        void parseAttribute(const std::string& line, UMLClass& cls) {
            UMLAttribute attr;
            
            // Parse visibility
            if (line.length() > 0) {
                attr.visibility = parseVisibility(line[0]);
            }

            std::string content = trim(line.substr(1));
            
            // Check for static
            if (content.find("{static}") != std::string::npos) {
                attr.isStatic = true;
                content = std::regex_replace(content, std::regex("\\{static\\}"), "");
                content = trim(content);
            }

            // Parse name : type
            size_t colonPos = content.find(":");
            if (colonPos != std::string::npos) {
                attr.name = trim(content.substr(0, colonPos));
                std::string typeStr = trim(content.substr(colonPos + 1));
                
                // Check for default value
                size_t equalPos = typeStr.find("=");
                if (equalPos != std::string::npos) {
                    attr.type = trim(typeStr.substr(0, equalPos));
                    attr.defaultValue = trim(typeStr.substr(equalPos + 1));
                } else {
                    attr.type = typeStr;
                }
            } else {
                attr.name = content;
            }

            cls.attributes.push_back(attr);
        }

        void parseMethod(const std::string& line, UMLClass& cls) {
            UMLMethod method;
            
            // Parse visibility
            if (line.length() > 0) {
                method.visibility = parseVisibility(line[0]);
            }

            std::string content = trim(line.substr(1));
            
            // Check for static
            if (content.find("{static}") != std::string::npos) {
                method.isStatic = true;
                content = std::regex_replace(content, std::regex("\\{static\\}"), "");
                content = trim(content);
            }

            // Check for abstract
            if (content.find("{abstract}") != std::string::npos) {
                method.isAbstract = true;
                content = std::regex_replace(content, std::regex("\\{abstract\\}"), "");
                content = trim(content);
            }

            // Parse method name and parameters
            size_t parenPos = content.find("(");
            if (parenPos != std::string::npos) {
                method.name = trim(content.substr(0, parenPos));
                
                size_t closeParenPos = content.find(")", parenPos);
                if (closeParenPos != std::string::npos) {
                    std::string params = content.substr(parenPos + 1, closeParenPos - parenPos - 1);
                    parseMethodParameters(params, method);
                    
                    // Parse return type
                    if (closeParenPos + 1 < content.length()) {
                        std::string rest = trim(content.substr(closeParenPos + 1));
                        if (rest.length() > 0 && rest[0] == ':') {
                            method.returnType = trim(rest.substr(1));
                        }
                    }
                }
            }

            cls.methods.push_back(method);
        }

        void parseMethodParameters(const std::string& paramsStr, UMLMethod& method) {
            if (trim(paramsStr).empty()) return;

            std::istringstream stream(paramsStr);
            std::string param;
            while (std::getline(stream, param, ',')) {
                param = trim(param);
                if (!param.empty()) {
                    UMLParameter paramInfo;
                    
                    // Parse parameter name : type
                    size_t colonPos = param.find(":");
                    if (colonPos != std::string::npos) {
                        paramInfo.name = trim(param.substr(0, colonPos));
                        paramInfo.type = trim(param.substr(colonPos + 1));
                    } else {
                        paramInfo.name = param;
                    }

                    method.parameters.push_back(paramInfo);
                }
            }
        }

        void parseRelationshipLine(const std::string& line) {
            UMLClassRelationship rel;
            
            // Find relationship symbols
            std::vector<std::pair<std::string, UMLRelationship>> patterns = {
                {"--|>", UMLRelationship::Inheritance},
                {"..|>", UMLRelationship::Realization},
                {"*--", UMLRelationship::Composition},
                {"o--", UMLRelationship::Aggregation},
                {"-->", UMLRelationship::DirectedAssociation},
                {"..>", UMLRelationship::Dependency},
                {"--", UMLRelationship::Association}
            };

            for (const auto& pattern : patterns) {
                size_t pos = line.find(pattern.first);
                if (pos != std::string::npos) {
                    rel.type = pattern.second;
                    
                    // Extract class names
                    rel.fromClass = trim(line.substr(0, pos));
                    std::string rest = trim(line.substr(pos + pattern.first.length()));
                    
                    // Extract label if present
                    size_t labelPos = rest.find(":");
                    if (labelPos != std::string::npos) {
                        rel.toClass = trim(rest.substr(0, labelPos));
                        rel.label = trim(rest.substr(labelPos + 1));
                    } else {
                        rel.toClass = rest;
                    }
                    
                    relationships.push_back(rel);
                    break;
                }
            }
        }

        void parseNote(const std::string& line) {
            // Simple note parsing
            if (line.find("note") != std::string::npos) {
                size_t ofPos = line.find(" of ");
                if (ofPos != std::string::npos) {
                    std::string className = trim(line.substr(ofPos + 4));
                    notes[className] = "Note"; // Simplified
                }
            }
        }
    };

    // PUMLClassParser implementation
    PUMLClassParser::PUMLClassParser() : pImpl(new Impl()) {}

    PUMLClassParser::~PUMLClassParser() = default;

    PUMLClassDiagramResult PUMLClassParser::parseFile(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            PUMLClassDiagramResult result;
            result.success = false;
            result.errorMessage = "Could not open file: " + filePath;
            return result;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        return parseContent(buffer.str());
    }

    PUMLClassDiagramResult PUMLClassParser::parseContent(const std::string& content) {
        return pImpl->parse(content);
    }

    const std::vector<UMLClass>& PUMLClassParser::getClasses() const {
        return pImpl->classes;
    }

    const std::vector<UMLClassRelationship>& PUMLClassParser::getRelationships() const {
        return pImpl->relationships;
    }

    const UMLClass* PUMLClassParser::findClass(const std::string& className) const {
        for (const auto& cls : pImpl->classes) {
            if (cls.name == className) {
                return &cls;
            }
        }
        return nullptr;
    }

    std::string PUMLClassParser::exportToJson() const {
        // Basic JSON export
        std::stringstream json;
        json << "{\n";
        json << "  \"classes\": [\n";
        
        for (size_t i = 0; i < pImpl->classes.size(); ++i) {
            const auto& cls = pImpl->classes[i];
            json << "    {\n";
            json << "      \"name\": \"" << cls.name << "\",\n";
            json << "      \"isAbstract\": " << (cls.isAbstract ? "true" : "false") << ",\n";
            json << "      \"isInterface\": " << (cls.isInterface ? "true" : "false") << "\n";
            json << "    }";
            if (i < pImpl->classes.size() - 1) json << ",";
            json << "\n";
        }
        
        json << "  ]\n";
        json << "}\n";
        return json.str();
    }

    std::string PUMLClassParser::exportToXML() const {
        // Basic XML export
        std::stringstream xml;
        xml << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        xml << "<ClassDiagram>\n";
        
        for (const auto& cls : pImpl->classes) {
            xml << "  <Class name=\"" << cls.name << "\" ";
            xml << "isAbstract=\"" << (cls.isAbstract ? "true" : "false") << "\" ";
            xml << "isInterface=\"" << (cls.isInterface ? "true" : "false") << "\" />\n";
        }
        
        xml << "</ClassDiagram>\n";
        return xml.str();
    }

    const std::vector<std::string>& PUMLClassParser::getWarnings() const {
        return pImpl->warnings;
    }

} // namespace UFMTooling
