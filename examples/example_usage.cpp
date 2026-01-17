#include "../include/SimpleHeaderParser.h"
#include "../include/PUMLClassParser.h"
#include "../include/PUMLEntityParser.h"
#include <iostream>
#include <iomanip>

using namespace UFMTooling;

void printSeparator() {
    std::cout << std::string(80, '=') << std::endl;
}

void demonstrateHeaderParser() {
    std::cout << "\n";
    printSeparator();
    std::cout << "SIMPLE HEADER PARSER EXAMPLE" << std::endl;
    printSeparator();

    // Example C++ header content
    std::string headerContent = R"(
#ifndef EXAMPLE_H
#define EXAMPLE_H

#include <string>
#include <vector>

// Base class
class Animal {
public:
    Animal(const std::string& name);
    virtual ~Animal();
    
    virtual void makeSound() = 0;
    std::string getName() const;
    
protected:
    std::string m_name;
    int m_age;
    
private:
    bool m_isAlive;
};

// Derived class
class Dog : public Animal {
public:
    Dog(const std::string& name, const std::string& breed);
    
    virtual void makeSound() override;
    void wagTail();
    
    static int getPopulation();
    
private:
    std::string m_breed;
    static int s_population;
};

// Another class
struct Point {
    double x;
    double y;
    double z;
    
    Point(double x = 0, double y = 0, double z = 0);
    double distance(const Point& other) const;
};

enum class Color {
    Red,
    Green,
    Blue
};

#endif // EXAMPLE_H
)";

    SimpleHeaderParser parser;
    ParseResult result = parser.parseContent(headerContent, "example.h");

    if (result.success) {
        std::cout << "\nParsing successful!" << std::endl;
        std::cout << "File: " << result.fileName << std::endl;
        std::cout << "\nIncludes found: " << result.includes.size() << std::endl;
        for (const auto& inc : result.includes) {
            std::cout << "  - " << inc << std::endl;
        }

        std::cout << "\nClasses found: " << result.classes.size() << std::endl;
        for (const auto& cls : result.classes) {
            std::cout << "\n  Class: " << cls.name << (cls.isStruct ? " (struct)" : " (class)") << std::endl;
            
            if (!cls.baseClasses.empty()) {
                std::cout << "  Base classes:" << std::endl;
                for (const auto& base : cls.baseClasses) {
                    std::cout << "    - " << base.name << std::endl;
                }
            }
            
            std::cout << "  Members: " << cls.members.size() << std::endl;
            for (const auto& member : cls.members) {
                std::cout << "    " << (member.isStatic ? "static " : "")
                          << member.type << " " << member.name << std::endl;
            }
            
            std::cout << "  Methods: " << cls.methods.size() << std::endl;
            for (const auto& method : cls.methods) {
                std::cout << "    " << (method.isVirtual ? "virtual " : "")
                          << (method.isStatic ? "static " : "")
                          << method.returnType << " " << method.name << "(";
                for (size_t i = 0; i < method.parameters.size(); ++i) {
                    if (i > 0) std::cout << ", ";
                    std::cout << method.parameters[i].type;
                }
                std::cout << ")" << (method.isConst ? " const" : "")
                          << (method.isPureVirtual ? " = 0" : "") << std::endl;
            }
        }

        std::cout << "\nEnums found: " << result.enums.size() << std::endl;
        for (const auto& e : result.enums) {
            std::cout << "  " << (e.isClass ? "enum class " : "enum ") << e.name << std::endl;
        }
    } else {
        std::cout << "Parsing failed: " << result.errorMessage << std::endl;
    }
}

void demonstratePUMLClassParser() {
    std::cout << "\n";
    printSeparator();
    std::cout << "PUML CLASS DIAGRAM PARSER EXAMPLE" << std::endl;
    printSeparator();

    // Example PlantUML class diagram
    std::string pumlContent = R"(
@startuml
title Class Diagram Example

class Vehicle {
    - brand : String
    - speed : int
    + Vehicle(brand : String)
    + accelerate() : void
    + brake() : void
    + getSpeed() : int
}

abstract class Car {
    - numberOfDoors : int
    + {abstract} openDoor() : void
}

interface Drivable {
    + drive() : void
    + stop() : void
}

class SportsCar {
    - turboEnabled : bool
    + enableTurbo() : void
}

Vehicle <|-- Car
Car <|-- SportsCar
Drivable <|.. Car

@enduml
)";

    PUMLClassParser parser;
    PUMLClassDiagramResult result = parser.parseContent(pumlContent);

    if (result.success) {
        std::cout << "\nParsing successful!" << std::endl;
        if (!result.title.empty()) {
            std::cout << "Title: " << result.title << std::endl;
        }

        std::cout << "\nClasses found: " << result.classes.size() << std::endl;
        for (const auto& cls : result.classes) {
            std::cout << "\n  Class: " << cls.name;
            if (cls.isAbstract) std::cout << " (abstract)";
            if (cls.isInterface) std::cout << " (interface)";
            std::cout << std::endl;
            
            if (!cls.stereotype.empty()) {
                std::cout << "  Stereotype: <<" << cls.stereotype << ">>" << std::endl;
            }
            
            std::cout << "  Attributes: " << cls.attributes.size() << std::endl;
            for (const auto& attr : cls.attributes) {
                std::cout << "    ";
                switch (attr.visibility) {
                    case UMLVisibility::Public: std::cout << "+"; break;
                    case UMLVisibility::Private: std::cout << "-"; break;
                    case UMLVisibility::Protected: std::cout << "#"; break;
                    case UMLVisibility::Package: std::cout << "~"; break;
                }
                std::cout << " " << attr.name;
                if (!attr.type.empty()) {
                    std::cout << " : " << attr.type;
                }
                std::cout << std::endl;
            }
            
            std::cout << "  Methods: " << cls.methods.size() << std::endl;
            for (const auto& method : cls.methods) {
                std::cout << "    ";
                switch (method.visibility) {
                    case UMLVisibility::Public: std::cout << "+"; break;
                    case UMLVisibility::Private: std::cout << "-"; break;
                    case UMLVisibility::Protected: std::cout << "#"; break;
                    case UMLVisibility::Package: std::cout << "~"; break;
                }
                std::cout << " " << method.name << "()" ;
                if (!method.returnType.empty()) {
                    std::cout << " : " << method.returnType;
                }
                std::cout << std::endl;
            }
        }

        std::cout << "\nRelationships found: " << result.relationships.size() << std::endl;
        for (const auto& rel : result.relationships) {
            std::cout << "  " << rel.fromClass << " --> " << rel.toClass;
            if (!rel.label.empty()) {
                std::cout << " : " << rel.label;
            }
            std::cout << std::endl;
        }

        // Demonstrate JSON export
        std::cout << "\nJSON Export:" << std::endl;
        std::cout << parser.exportToJson() << std::endl;
    } else {
        std::cout << "Parsing failed: " << result.errorMessage << std::endl;
    }
}

void demonstratePUMLEntityParser() {
    std::cout << "\n";
    printSeparator();
    std::cout << "PUML ENTITY DIAGRAM PARSER EXAMPLE" << std::endl;
    printSeparator();

    // Example PlantUML entity diagram
    std::string pumlContent = R"(
@startuml
title Entity Relationship Diagram

entity Customer {
    * customer_id : int
    --
    name : varchar
    email : varchar
    phone : varchar
}

entity Order {
    * order_id : int
    --
    + customer_id : int
    order_date : date
    total_amount : decimal
}

entity OrderItem {
    * item_id : int
    --
    + order_id : int
    + product_id : int
    quantity : int
    price : decimal
}

entity Product {
    * product_id : int
    --
    name : varchar
    description : text
    price : decimal
    stock : int
}

Customer ||--o{ Order
Order ||--o{ OrderItem
Product ||--o{ OrderItem

@enduml
)";

    PUMLEntityParser parser;
    PUMLEntityDiagramResult result = parser.parseContent(pumlContent);

    if (result.success) {
        std::cout << "\nParsing successful!" << std::endl;
        if (!result.title.empty()) {
            std::cout << "Title: " << result.title << std::endl;
        }

        std::cout << "\nEntities found: " << result.entities.size() << std::endl;
        for (const auto& entity : result.entities) {
            std::cout << "\n  Entity: " << entity.name << std::endl;
            std::cout << "  Fields: " << entity.fields.size() << std::endl;
            for (const auto& field : entity.fields) {
                std::cout << "    ";
                if (field.isPrimaryKey) std::cout << "* ";
                else if (field.isForeignKey) std::cout << "+ ";
                else std::cout << "  ";
                std::cout << field.name;
                if (!field.type.empty()) {
                    std::cout << " : " << field.type;
                }
                if (field.isPrimaryKey) std::cout << " (PK)";
                if (field.isForeignKey) std::cout << " (FK)";
                std::cout << std::endl;
            }
        }

        std::cout << "\nRelationships found: " << result.relationships.size() << std::endl;
        for (const auto& rel : result.relationships) {
            std::cout << "  " << rel.fromEntity << " --> " << rel.toEntity;
            if (!rel.label.empty()) {
                std::cout << " : " << rel.label;
            }
            std::cout << std::endl;
        }

        // Demonstrate DDL export
        std::cout << "\nSQL DDL Export:" << std::endl;
        std::cout << parser.exportToDDL("SQL") << std::endl;
    } else {
        std::cout << "Parsing failed: " << result.errorMessage << std::endl;
    }
}

int main2() {
    std::cout << "\n";
    printSeparator();
    std::cout << "UFM-TOOLING LIBRARY DEMONSTRATION" << std::endl;
    printSeparator();

    try {
        // Demonstrate each parser
        demonstrateHeaderParser();
        demonstratePUMLClassParser();
        demonstratePUMLEntityParser();

        std::cout << "\n";
        printSeparator();
        std::cout << "ALL DEMONSTRATIONS COMPLETED SUCCESSFULLY" << std::endl;
        printSeparator();
        std::cout << "\n";

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
