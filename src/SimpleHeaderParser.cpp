#include "../include/SimpleHeaderParser.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <regex>

namespace UFMTooling {

    // Helper functions for parsing
    namespace {
        // Trim whitespace from both ends
        std::string trim(const std::string& str) {
            size_t first = str.find_first_not_of(" \t\n\r");
            if (first == std::string::npos) return "";
            size_t last = str.find_last_not_of(" \t\n\r");
            return str.substr(first, last - first + 1);
        }

        // Check if a line is a comment
        bool isComment(const std::string& line) {
            std::string trimmed = trim(line);
            return trimmed.empty() || 
                   trimmed.find("//") == 0 || 
                   trimmed.find("/*") == 0 ||
                   trimmed.find("*") == 0;
        }

        // Remove comments from a line
        std::string removeComments(const std::string& line) {
            size_t pos = line.find("//");
            if (pos != std::string::npos) {
                return line.substr(0, pos);
            }
            return line;
        }

        // Parse access specifier
        AccessSpecifier parseAccessSpecifier(const std::string& str) {
            std::string s = trim(str);
            if (s == "public" || s == "public:") return AccessSpecifier::Public;
            if (s == "protected" || s == "protected:") return AccessSpecifier::Protected;
            if (s == "private" || s == "private:") return AccessSpecifier::Private;
            return AccessSpecifier::None;
        }

        // Check if a string contains a keyword
        bool containsKeyword(const std::string& str, const std::string& keyword) {
            return str.find(keyword) != std::string::npos;
        }
    }

    // Implementation class (Pimpl pattern)
    class SimpleHeaderParser::Impl {
    public:
        std::vector<ClassInfo> classes;
        std::vector<NamespaceInfo> namespaces;
        std::vector<EnumInfo> enums;
        std::vector<std::string> warnings;
        ParseResult lastResult;

        ParseResult parse(const std::string& content, const std::string& fileName) {
            ParseResult result;
            result.fileName = fileName;
            result.success = true;

            classes.clear();
            namespaces.clear();
            enums.clear();
            warnings.clear();

            try {
                std::istringstream stream(content);
                std::string line;
                std::vector<std::string> lines;

                // Read all lines
                while (std::getline(stream, line)) {
                    lines.push_back(line);
                }

                // Parse includes
                parseIncludes(lines, result);

                // Parse classes and structs
                parseClasses(lines, result);

                // Parse enums
                parseEnums(lines, result);

                result.classes = classes;
                result.namespaces = namespaces;
                result.enums = enums;

            } catch (const std::exception& e) {
                result.success = false;
                result.errorMessage = std::string("Parsing error: ") + e.what();
            }

            lastResult = result;
            return result;
        }

    private:
        void parseIncludes(const std::vector<std::string>& lines, ParseResult& result) {
            std::regex includeRegex(R"(^\s*#\s*include\s*[<"]([^>"]+)[>"])");
            for (const auto& line : lines) {
                std::smatch match;
                if (std::regex_search(line, match, includeRegex)) {
                    result.includes.push_back(match[1].str());
                }
            }
        }

        void parseClasses(const std::vector<std::string>& lines, ParseResult& result) {
            (void)result; // Suppress unused parameter warning
            for (size_t i = 0; i < lines.size(); ++i) {
                std::string line = trim(removeComments(lines[i]));
                
                // Check for class or struct declaration
                if ((line.find("class ") != std::string::npos || 
                     line.find("struct ") != std::string::npos) &&
                    line.find(";") == std::string::npos) { // Not a forward declaration
                    
                    ClassInfo classInfo;
                    classInfo.isStruct = line.find("struct ") != std::string::npos;
                    
                    // Find class name
                    size_t classPos = line.find(classInfo.isStruct ? "struct " : "class ");
                    if (classPos != std::string::npos) {
                        std::string rest = line.substr(classPos + (classInfo.isStruct ? 7 : 6));
                        rest = trim(rest);
                        
                        // Extract class name
                        size_t nameEnd = rest.find_first_of(" :{");
                        if (nameEnd != std::string::npos) {
                            classInfo.name = rest.substr(0, nameEnd);
                        } else {
                            classInfo.name = rest;
                        }

                        // Parse inheritance
                        size_t colonPos = line.find(":");
                        if (colonPos != std::string::npos) {
                            parseBaseClasses(line.substr(colonPos + 1), classInfo);
                        }

                        // Parse class body
                        i = parseClassBody(lines, i + 1, classInfo);
                        
                        classes.push_back(classInfo);
                    }
                }
            }
        }

        void parseBaseClasses(const std::string& baseStr, ClassInfo& classInfo) {
            std::string bases = trim(baseStr);
            size_t bracePos = bases.find("{");
            if (bracePos != std::string::npos) {
                bases = bases.substr(0, bracePos);
            }

            // Split by comma
            std::istringstream stream(bases);
            std::string baseClass;
            while (std::getline(stream, baseClass, ',')) {
                baseClass = trim(baseClass);
                if (!baseClass.empty()) {
                    BaseClassInfo base;
                    
                    // Check for access specifier
                    if (baseClass.find("public ") == 0) {
                        base.access = AccessSpecifier::Public;
                        base.name = trim(baseClass.substr(7));
                    } else if (baseClass.find("protected ") == 0) {
                        base.access = AccessSpecifier::Protected;
                        base.name = trim(baseClass.substr(10));
                    } else if (baseClass.find("private ") == 0) {
                        base.access = AccessSpecifier::Private;
                        base.name = trim(baseClass.substr(8));
                    } else {
                        base.access = AccessSpecifier::Public;
                        base.name = baseClass;
                    }
                    
                    classInfo.baseClasses.push_back(base);
                }
            }
        }

        size_t parseClassBody(const std::vector<std::string>& lines, size_t startIdx, ClassInfo& classInfo) {
            AccessSpecifier currentAccess = classInfo.isStruct ? AccessSpecifier::Public : AccessSpecifier::Private;
            int braceLevel = 0;
            bool inClass = false;

            for (size_t i = startIdx; i < lines.size(); ++i) {
                std::string line = trim(removeComments(lines[i]));
                
                if (line.empty()) continue;

                // Count braces
                for (char c : line) {
                    if (c == '{') {
                        braceLevel++;
                        inClass = true;
                    } else if (c == '}') {
                        braceLevel--;
                        if (braceLevel == 0 && inClass) {
                            return i;
                        }
                    }
                }

                if (!inClass || braceLevel != 1) continue;

                // Check for access specifier
                if (line.find("public:") != std::string::npos) {
                    currentAccess = AccessSpecifier::Public;
                    continue;
                } else if (line.find("protected:") != std::string::npos) {
                    currentAccess = AccessSpecifier::Protected;
                    continue;
                } else if (line.find("private:") != std::string::npos) {
                    currentAccess = AccessSpecifier::Private;
                    continue;
                }

                // Parse member or method
                if (line.find("(") != std::string::npos && line.find(")") != std::string::npos) {
                    // Likely a method
                    parseMethod(line, currentAccess, classInfo);
                } else if (line.find(";") != std::string::npos) {
                    // Likely a member variable
                    parseMember(line, currentAccess, classInfo);
                }
            }

            return lines.size();
        }

        void parseMethod(const std::string& line, AccessSpecifier access, ClassInfo& classInfo) {
            MethodInfo method;
            method.access = access;

            std::string cleanLine = trim(line);
            
            // Check for modifiers
            method.isStatic = containsKeyword(cleanLine, "static");
            method.isVirtual = containsKeyword(cleanLine, "virtual");
            method.isPureVirtual = containsKeyword(cleanLine, "= 0");
            method.isConst = cleanLine.find(")") != std::string::npos && 
                           cleanLine.find("const", cleanLine.find(")")) != std::string::npos;

            // Extract method signature
            size_t parenPos = cleanLine.find("(");
            if (parenPos != std::string::npos) {
                std::string beforeParen = cleanLine.substr(0, parenPos);
                
                // Remove modifiers
                beforeParen = std::regex_replace(beforeParen, std::regex("\\bstatic\\b"), "");
                beforeParen = std::regex_replace(beforeParen, std::regex("\\bvirtual\\b"), "");
                beforeParen = std::regex_replace(beforeParen, std::regex("\\binline\\b"), "");
                beforeParen = trim(beforeParen);

                // Split return type and method name
                size_t lastSpace = beforeParen.find_last_of(" \t");
                if (lastSpace != std::string::npos) {
                    method.returnType = trim(beforeParen.substr(0, lastSpace));
                    method.name = trim(beforeParen.substr(lastSpace + 1));
                } else {
                    method.name = beforeParen; // Constructor or destructor
                    method.isConstructor = (method.name == classInfo.name);
                    method.isDestructor = (method.name == "~" + classInfo.name);
                }

                // Parse parameters
                size_t closeParen = cleanLine.find(")", parenPos);
                if (closeParen != std::string::npos) {
                    std::string params = cleanLine.substr(parenPos + 1, closeParen - parenPos - 1);
                    parseParameters(params, method);
                }
            }

            classInfo.methods.push_back(method);
        }

        void parseParameters(const std::string& paramsStr, MethodInfo& method) {
            if (trim(paramsStr).empty()) return;

            std::istringstream stream(paramsStr);
            std::string param;
            while (std::getline(stream, param, ',')) {
                param = trim(param);
                if (!param.empty()) {
                    ParameterInfo paramInfo;
                    
                    paramInfo.isConst = containsKeyword(param, "const");
                    paramInfo.isReference = containsKeyword(param, "&");
                    paramInfo.isPointer = containsKeyword(param, "*");

                    // Simple parameter parsing
                    size_t lastSpace = param.find_last_of(" \t*&");
                    if (lastSpace != std::string::npos) {
                        paramInfo.type = trim(param.substr(0, lastSpace + 1));
                        std::string nameAndDefault = trim(param.substr(lastSpace + 1));
                        
                        size_t equalPos = nameAndDefault.find("=");
                        if (equalPos != std::string::npos) {
                            paramInfo.name = trim(nameAndDefault.substr(0, equalPos));
                            paramInfo.defaultValue = trim(nameAndDefault.substr(equalPos + 1));
                        } else {
                            paramInfo.name = nameAndDefault;
                        }
                    }

                    method.parameters.push_back(paramInfo);
                }
            }
        }

        void parseMember(const std::string& line, AccessSpecifier access, ClassInfo& classInfo) {
            MemberInfo member;
            member.access = access;

            std::string cleanLine = trim(line);
            
            // Check for modifiers
            member.isStatic = containsKeyword(cleanLine, "static");
            member.isConst = containsKeyword(cleanLine, "const");

            // Remove semicolon
            if (cleanLine.back() == ';') {
                cleanLine = cleanLine.substr(0, cleanLine.length() - 1);
            }

            // Remove modifiers
            cleanLine = std::regex_replace(cleanLine, std::regex("\\bstatic\\b"), "");
            cleanLine = std::regex_replace(cleanLine, std::regex("\\bmutable\\b"), "");
            cleanLine = trim(cleanLine);

            // Extract type and name
            size_t lastSpace = cleanLine.find_last_of(" \t");
            if (lastSpace != std::string::npos) {
                member.type = trim(cleanLine.substr(0, lastSpace));
                std::string nameAndDefault = trim(cleanLine.substr(lastSpace + 1));
                
                size_t equalPos = nameAndDefault.find("=");
                if (equalPos != std::string::npos) {
                    member.name = trim(nameAndDefault.substr(0, equalPos));
                    member.defaultValue = trim(nameAndDefault.substr(equalPos + 1));
                } else {
                    member.name = nameAndDefault;
                }
            }

            if (!member.name.empty()) {
                classInfo.members.push_back(member);
            }
        }

        void parseEnums(const std::vector<std::string>& lines, ParseResult& result) {
            (void)result; // Suppress unused parameter warning
            for (size_t i = 0; i < lines.size(); ++i) {
                std::string line = trim(removeComments(lines[i]));
                
                if (line.find("enum") != std::string::npos) {
                    EnumInfo enumInfo;
                    enumInfo.isClass = line.find("enum class") != std::string::npos;
                    
                    // Extract enum name
                    size_t enumPos = line.find(enumInfo.isClass ? "enum class" : "enum");
                    if (enumPos != std::string::npos) {
                        std::string rest = line.substr(enumPos + (enumInfo.isClass ? 10 : 4));
                        rest = trim(rest);
                        
                        size_t nameEnd = rest.find_first_of(" :{");
                        if (nameEnd != std::string::npos) {
                            enumInfo.name = rest.substr(0, nameEnd);
                        } else {
                            enumInfo.name = rest;
                        }

                        // Parse enum values (basic implementation)
                        enums.push_back(enumInfo);
                    }
                }
            }
        }
    };

    // SimpleHeaderParser implementation
    SimpleHeaderParser::SimpleHeaderParser() : pImpl(new Impl()) {}

    SimpleHeaderParser::~SimpleHeaderParser() = default;

    ParseResult SimpleHeaderParser::parseFile(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            ParseResult result;
            result.success = false;
            result.errorMessage = "Could not open file: " + filePath;
            result.fileName = filePath;
            return result;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        return parseContent(buffer.str(), filePath);
    }

    ParseResult SimpleHeaderParser::parseContent(const std::string& content, const std::string& fileName) {
        return pImpl->parse(content, fileName);
    }

    const std::vector<ClassInfo>& SimpleHeaderParser::getClasses() const {
        return pImpl->classes;
    }

    const std::vector<NamespaceInfo>& SimpleHeaderParser::getNamespaces() const {
        return pImpl->namespaces;
    }

    const std::vector<EnumInfo>& SimpleHeaderParser::getEnums() const {
        return pImpl->enums;
    }

    const ClassInfo* SimpleHeaderParser::findClass(const std::string& className) const {
        for (const auto& cls : pImpl->classes) {
            if (cls.name == className) {
                return &cls;
            }
        }
        return nullptr;
    }

    const std::vector<std::string>& SimpleHeaderParser::getWarnings() const {
        return pImpl->warnings;
    }

} // namespace UFMTooling
