#include "../include/SimpleHeaderParser.h"
#include "../include/PUMLClassParser.h"
#include "../include/PUMLEntityParser.h"
#include <iostream>

using namespace UFMTooling;

int main() {
    std::cout << "Testing parsers with sample files...\n\n";

    // Test SimpleHeaderParser
    std::cout << "1. Testing SimpleHeaderParser with sample_header.h\n";
    SimpleHeaderParser headerParser;
    ParseResult headerResult = headerParser.parseFile("examples/sample_header.h");
    if (headerResult.success) {
        std::cout << "   SUCCESS: Found " << headerResult.classes.size() << " classes\n";
        for (const auto& cls : headerResult.classes) {
            std::cout << "   - " << cls.name 
                      << " (" << cls.members.size() << " members, " 
                      << cls.methods.size() << " methods)\n";
        }
    } else {
        std::cout << "   FAILED: " << headerResult.errorMessage << "\n";
    }

    // Test PUMLClassParser
    std::cout << "\n2. Testing PUMLClassParser with sample_class_diagram.puml\n";
    PUMLClassParser classParser;
    PUMLClassDiagramResult classResult = classParser.parseFile("examples/sample_class_diagram.puml");
    if (classResult.success) {
        std::cout << "   SUCCESS: Found " << classResult.classes.size() << " classes\n";
        for (const auto& cls : classResult.classes) {
            std::cout << "   - " << cls.name;
            if (cls.isAbstract) std::cout << " (abstract)";
            if (cls.isInterface) std::cout << " (interface)";
            std::cout << "\n";
        }
        std::cout << "   Found " << classResult.relationships.size() << " relationships\n";
    } else {
        std::cout << "   FAILED: " << classResult.errorMessage << "\n";
    }

    // Test PUMLEntityParser
    std::cout << "\n3. Testing PUMLEntityParser with sample_entity_diagram.puml\n";
    PUMLEntityParser entityParser;
    PUMLEntityDiagramResult entityResult = entityParser.parseFile("examples/sample_entity_diagram.puml");
    if (entityResult.success) {
        std::cout << "   SUCCESS: Found " << entityResult.entities.size() << " entities\n";
        for (const auto& entity : entityResult.entities) {
            std::cout << "   - " << entity.name 
                      << " (" << entity.fields.size() << " fields)\n";
        }
        std::cout << "   Found " << entityResult.relationships.size() << " relationships\n";
    } else {
        std::cout << "   FAILED: " << entityResult.errorMessage << "\n";
    }

    std::cout << "\nAll tests completed!\n";
    return 0;
}
