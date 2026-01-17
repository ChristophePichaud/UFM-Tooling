#ifndef SIMPLE_HEADER_PARSER_H
#define SIMPLE_HEADER_PARSER_H

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace UFMTooling {

    // Enum for access specifiers
    enum class AccessSpecifier {
        Public,
        Protected,
        Private,
        None
    };

    // Represents a member variable in a class
    struct MemberInfo {
        std::string name;
        std::string type;
        AccessSpecifier access;
        bool isStatic;
        bool isConst;
        std::string defaultValue;

        MemberInfo() : access(AccessSpecifier::Private), isStatic(false), isConst(false) {}
    };

    // Represents a parameter in a method
    struct ParameterInfo {
        std::string name;
        std::string type;
        std::string defaultValue;
        bool isConst;
        bool isReference;
        bool isPointer;

        ParameterInfo() : isConst(false), isReference(false), isPointer(false) {}
    };

    // Represents a method/function in a class
    struct MethodInfo {
        std::string name;
        std::string returnType;
        AccessSpecifier access;
        std::vector<ParameterInfo> parameters;
        bool isStatic;
        bool isConst;
        bool isVirtual;
        bool isPureVirtual;
        bool isConstructor;
        bool isDestructor;
        bool isOperator;

        MethodInfo() : access(AccessSpecifier::Private), isStatic(false), isConst(false),
                       isVirtual(false), isPureVirtual(false), isConstructor(false),
                       isDestructor(false), isOperator(false) {}
    };

    // Represents a base class in inheritance
    struct BaseClassInfo {
        std::string name;
        AccessSpecifier access;

        BaseClassInfo() : access(AccessSpecifier::Public) {}
    };

    // Represents a C++ class
    struct ClassInfo {
        std::string name;
        std::string fullName; // Including namespace
        std::vector<BaseClassInfo> baseClasses;
        std::vector<MemberInfo> members;
        std::vector<MethodInfo> methods;
        std::vector<std::string> friendClasses;
        bool isStruct;
        bool isTemplate;
        std::vector<std::string> templateParameters;

        ClassInfo() : isStruct(false), isTemplate(false) {}
    };

    // Represents a namespace
    struct NamespaceInfo {
        std::string name;
        std::vector<ClassInfo> classes;
        std::vector<NamespaceInfo> nestedNamespaces;
    };

    // Represents an enum
    struct EnumInfo {
        std::string name;
        std::vector<std::pair<std::string, std::string>> values; // name, value
        bool isClass; // enum class vs enum

        EnumInfo() : isClass(false) {}
    };

    // Main parser result
    struct ParseResult {
        std::vector<ClassInfo> classes;
        std::vector<NamespaceInfo> namespaces;
        std::vector<EnumInfo> enums;
        std::vector<std::string> includes;
        std::string fileName;
        bool success;
        std::string errorMessage;

        ParseResult() : success(false) {}
    };

    // Main parser class
    class SimpleHeaderParser {
    public:
        SimpleHeaderParser();
        ~SimpleHeaderParser();

        // Parse a header file from path
        ParseResult parseFile(const std::string& filePath);

        // Parse header content from string
        ParseResult parseContent(const std::string& content, const std::string& fileName = "");

        // Get all classes found
        const std::vector<ClassInfo>& getClasses() const;

        // Get all namespaces found
        const std::vector<NamespaceInfo>& getNamespaces() const;

        // Get all enums found
        const std::vector<EnumInfo>& getEnums() const;

        // Find a class by name
        const ClassInfo* findClass(const std::string& className) const;

        // Get parsing errors/warnings
        const std::vector<std::string>& getWarnings() const;

    private:
        class Impl;
        std::unique_ptr<Impl> pImpl;
    };

} // namespace UFMTooling

#endif // SIMPLE_HEADER_PARSER_H
