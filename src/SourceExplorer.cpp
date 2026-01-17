#include "../include/SourceExplorer.h"
#include "../include/FileSystemExplorer.h"
#include "../include/SimpleHeaderParser.h"
#include "../include/third_party/json.hpp"
#include <fstream>

using json = nlohmann::json;

namespace UFMTooling {

    class SourceExplorer::Impl {
    public:
        SourceExplorerResult lastResult;
        FileSystemExplorer fsExplorer;
        SimpleHeaderParser parser;

        SourceExplorerResult exploreSource(const std::string& basePath, bool bRecursive) {
            SourceExplorerResult result;
            
            // Use FileSystemExplorer to get all files
            FileSystemExplorerResult fsResult = fsExplorer.explore(basePath, bRecursive);
            
            if (!fsResult.success) {
                result.errorMessage = fsResult.errorMessage;
                return result;
            }
            
            // Filter for .h files
            std::vector<FileSystemEntry> headerFiles = fsExplorer.getFilesByExtension(".h");
            
            result.filesProcessed = 0;
            result.filesWithErrors = 0;
            
            // Parse each header file
            for (const auto& headerFile : headerFiles) {
                SourceFileAnalysis analysis;
                analysis.path = headerFile.path;
                analysis.filename = headerFile.name;
                
                // Parse the header file
                ParseResult parseResult = parser.parseFile(headerFile.path);
                analysis.parseResult = parseResult;
                analysis.success = parseResult.success;
                
                if (!parseResult.success) {
                    analysis.errorMessage = parseResult.errorMessage;
                    result.filesWithErrors++;
                } else {
                    analysis.errorMessage = "";
                }
                
                result.analyses.push_back(analysis);
                result.filesProcessed++;
            }
            
            result.success = true;
            lastResult = result;
            return result;
        }

        std::string convertToJson() const {
            json j;
            
            j["success"] = lastResult.success;
            j["filesProcessed"] = lastResult.filesProcessed;
            j["filesWithErrors"] = lastResult.filesWithErrors;
            j["errorMessage"] = lastResult.errorMessage;
            
            json filesArray = json::array();
            
            for (const auto& analysis : lastResult.analyses) {
                json fileJson;
                fileJson["path"] = analysis.path;
                fileJson["filename"] = analysis.filename;
                fileJson["success"] = analysis.success;
                fileJson["errorMessage"] = analysis.errorMessage;
                
                // Add parse results
                json classesArray = json::array();
                for (const auto& cls : analysis.parseResult.classes) {
                    json classJson;
                    classJson["name"] = cls.name;
                    classJson["fullName"] = cls.fullName;
                    classJson["isStruct"] = cls.isStruct;
                    classJson["isTemplate"] = cls.isTemplate;
                    
                    // Add base classes
                    json baseClassesArray = json::array();
                    for (const auto& base : cls.baseClasses) {
                        json baseJson;
                        baseJson["name"] = base.name;
                        baseJson["access"] = accessSpecifierToString(base.access);
                        baseClassesArray.push_back(baseJson);
                    }
                    classJson["baseClasses"] = baseClassesArray;
                    
                    // Add members
                    json membersArray = json::array();
                    for (const auto& member : cls.members) {
                        json memberJson;
                        memberJson["name"] = member.name;
                        memberJson["type"] = member.type;
                        memberJson["access"] = accessSpecifierToString(member.access);
                        memberJson["isStatic"] = member.isStatic;
                        memberJson["isConst"] = member.isConst;
                        memberJson["defaultValue"] = member.defaultValue;
                        membersArray.push_back(memberJson);
                    }
                    classJson["members"] = membersArray;
                    
                    // Add methods
                    json methodsArray = json::array();
                    for (const auto& method : cls.methods) {
                        json methodJson;
                        methodJson["name"] = method.name;
                        methodJson["returnType"] = method.returnType;
                        methodJson["access"] = accessSpecifierToString(method.access);
                        methodJson["isStatic"] = method.isStatic;
                        methodJson["isConst"] = method.isConst;
                        methodJson["isVirtual"] = method.isVirtual;
                        methodJson["isPureVirtual"] = method.isPureVirtual;
                        methodJson["isConstructor"] = method.isConstructor;
                        methodJson["isDestructor"] = method.isDestructor;
                        methodJson["isOperator"] = method.isOperator;
                        
                        // Add parameters
                        json parametersArray = json::array();
                        for (const auto& param : method.parameters) {
                            json paramJson;
                            paramJson["name"] = param.name;
                            paramJson["type"] = param.type;
                            paramJson["defaultValue"] = param.defaultValue;
                            paramJson["isConst"] = param.isConst;
                            paramJson["isReference"] = param.isReference;
                            paramJson["isPointer"] = param.isPointer;
                            parametersArray.push_back(paramJson);
                        }
                        methodJson["parameters"] = parametersArray;
                        
                        methodsArray.push_back(methodJson);
                    }
                    classJson["methods"] = methodsArray;
                    
                    // Add template parameters
                    json templateParamsArray = json::array();
                    for (const auto& tparam : cls.templateParameters) {
                        templateParamsArray.push_back(tparam);
                    }
                    classJson["templateParameters"] = templateParamsArray;
                    
                    // Add friend classes
                    json friendClassesArray = json::array();
                    for (const auto& friendClass : cls.friendClasses) {
                        friendClassesArray.push_back(friendClass);
                    }
                    classJson["friendClasses"] = friendClassesArray;
                    
                    classesArray.push_back(classJson);
                }
                fileJson["classes"] = classesArray;
                
                // Add enums
                json enumsArray = json::array();
                for (const auto& enumInfo : analysis.parseResult.enums) {
                    json enumJson;
                    enumJson["name"] = enumInfo.name;
                    enumJson["isClass"] = enumInfo.isClass;
                    
                    json valuesArray = json::array();
                    for (const auto& value : enumInfo.values) {
                        json valueJson;
                        valueJson["name"] = value.first;
                        valueJson["value"] = value.second;
                        valuesArray.push_back(valueJson);
                    }
                    enumJson["values"] = valuesArray;
                    
                    enumsArray.push_back(enumJson);
                }
                fileJson["enums"] = enumsArray;
                
                // Add includes
                json includesArray = json::array();
                for (const auto& include : analysis.parseResult.includes) {
                    includesArray.push_back(include);
                }
                fileJson["includes"] = includesArray;
                
                filesArray.push_back(fileJson);
            }
            
            j["files"] = filesArray;
            
            return j.dump(2); // Pretty print with 2 spaces indentation
        }

        bool saveJsonToFile(const std::string& filePath) const {
            try {
                std::ofstream outFile(filePath);
                if (!outFile.is_open()) {
                    return false;
                }
                
                outFile << convertToJson();
                outFile.close();
                return true;
            } catch (const std::exception&) {
                return false;
            }
        }

    private:
        std::string accessSpecifierToString(AccessSpecifier access) const {
            switch (access) {
                case AccessSpecifier::Public: return "public";
                case AccessSpecifier::Protected: return "protected";
                case AccessSpecifier::Private: return "private";
                case AccessSpecifier::None: return "none";
                default: return "unknown";
            }
        }
    };

    // SourceExplorer implementation
    SourceExplorer::SourceExplorer() : pImpl(new Impl()) {}

    SourceExplorer::~SourceExplorer() = default;

    SourceExplorerResult SourceExplorer::explore(const std::string& basePath, bool bRecursive) {
        return pImpl->exploreSource(basePath, bRecursive);
    }

    std::string SourceExplorer::exportToJson() const {
        return pImpl->convertToJson();
    }

    bool SourceExplorer::exportToJsonFile(const std::string& filePath) const {
        return pImpl->saveJsonToFile(filePath);
    }

    const SourceExplorerResult& SourceExplorer::getLastResult() const {
        return pImpl->lastResult;
    }

} // namespace UFMTooling
