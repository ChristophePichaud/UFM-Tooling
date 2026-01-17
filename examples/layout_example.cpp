#include "ShapeElement.h"
#include "LayoutEngine.h"
#include <iostream>
#include <vector>
#include <memory>
#include <iomanip>

using namespace UFMTooling;

void printElementPositions(const std::vector<std::shared_ptr<ShapeElement>>& elements) {
    std::cout << "\nElement Positions:\n";
    std::cout << std::string(70, '-') << "\n";
    
    for (const auto& elem : elements) {
        if (elem->getElementType() == ElementType::Drawing) {
            auto drawing = std::static_pointer_cast<DrawingElement>(elem);
            std::cout << std::setw(20) << drawing->getName() 
                     << " at (" << std::fixed << std::setprecision(1) 
                     << elem->getPosition().x << ", " 
                     << elem->getPosition().y << ")\n";
        }
    }
    std::cout << std::string(70, '-') << "\n";
}

int main() {
    std::cout << "=== UFM-Tooling Layout Engine Example ===\n\n";

    // Create drawing elements
    auto class1 = std::make_shared<DrawingElement>("UserClass");
    class1->setShapeType("class");
    class1->setColor("#ADD8E6");
    class1->setSize(120.0, 80.0);

    auto class2 = std::make_shared<DrawingElement>("OrderClass");
    class2->setShapeType("class");
    class2->setColor("#90EE90");
    class2->setSize(120.0, 80.0);

    auto class3 = std::make_shared<DrawingElement>("ProductClass");
    class3->setShapeType("class");
    class3->setColor("#FFB6C1");
    class3->setSize(120.0, 80.0);

    auto class4 = std::make_shared<DrawingElement>("PaymentClass");
    class4->setShapeType("class");
    class4->setColor("#FFFFE0");
    class4->setSize(120.0, 80.0);

    auto class5 = std::make_shared<DrawingElement>("ShippingClass");
    class5->setShapeType("class");
    class5->setColor("#FFD700");
    class5->setSize(120.0, 80.0);

    // Create relationships
    auto rel1 = std::make_shared<RelationshipElement>(class1, class2);
    rel1->setRelationshipType("association");
    rel1->setLabel("places");

    auto rel2 = std::make_shared<RelationshipElement>(class2, class3);
    rel2->setRelationshipType("contains");
    rel2->setLabel("contains");

    auto rel3 = std::make_shared<RelationshipElement>(class2, class4);
    rel3->setRelationshipType("uses");
    rel3->setLabel("pays with");

    auto rel4 = std::make_shared<RelationshipElement>(class2, class5);
    rel4->setRelationshipType("uses");
    rel4->setLabel("ships via");

    // Create vector of all elements
    std::vector<std::shared_ptr<ShapeElement>> elements;
    elements.push_back(class1);
    elements.push_back(class2);
    elements.push_back(class3);
    elements.push_back(class4);
    elements.push_back(class5);
    elements.push_back(rel1);
    elements.push_back(rel2);
    elements.push_back(rel3);
    elements.push_back(rel4);

    // Create layout engine
    CanvasSize canvas(1600.0, 900.0);
    LayoutEngine engine(canvas);

    std::cout << "Canvas size: " << canvas.width << " x " << canvas.height << "\n";
    std::cout << "Total elements: " << elements.size() 
             << " (" << 5 << " drawing, " << 4 << " relationships)\n";

    // Test different layout strategies
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "1. GRID LAYOUT\n";
    std::cout << std::string(70, '=') << "\n";
    
    LayoutConfig gridConfig;
    gridConfig.strategy = LayoutStrategy::Grid;
    gridConfig.padding = 30.0;
    
    LayoutResult result = engine.arrangeElements(elements, gridConfig);
    std::cout << "Success: " << (result.success ? "Yes" : "No") << "\n";
    std::cout << "Elements arranged: " << result.elementsArranged << "\n";
    std::cout << "Total area used: " << result.totalArea << " sq units\n";
    
    printElementPositions(elements);

    int overlaps = engine.countOverlaps(elements);
    std::cout << "Overlapping elements: " << overlaps << "\n";

    // Test hierarchical layout
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "2. HIERARCHICAL LAYOUT\n";
    std::cout << std::string(70, '=') << "\n";
    
    LayoutConfig hierarchicalConfig;
    hierarchicalConfig.strategy = LayoutStrategy::Hierarchical;
    hierarchicalConfig.padding = 40.0;
    
    result = engine.arrangeElements(elements, hierarchicalConfig);
    std::cout << "Success: " << (result.success ? "Yes" : "No") << "\n";
    std::cout << "Elements arranged: " << result.elementsArranged << "\n";
    std::cout << "Total area used: " << result.totalArea << " sq units\n";
    
    printElementPositions(elements);

    overlaps = engine.countOverlaps(elements);
    std::cout << "Overlapping elements: " << overlaps << "\n";

    // Test circular layout
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "3. CIRCULAR LAYOUT\n";
    std::cout << std::string(70, '=') << "\n";
    
    LayoutConfig circularConfig;
    circularConfig.strategy = LayoutStrategy::Circular;
    circularConfig.padding = 20.0;
    
    result = engine.arrangeElements(elements, circularConfig);
    std::cout << "Success: " << (result.success ? "Yes" : "No") << "\n";
    std::cout << "Elements arranged: " << result.elementsArranged << "\n";
    std::cout << "Total area used: " << result.totalArea << " sq units\n";
    
    printElementPositions(elements);

    overlaps = engine.countOverlaps(elements);
    std::cout << "Overlapping elements: " << overlaps << "\n";

    // Test force-directed layout
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "4. FORCE-DIRECTED LAYOUT\n";
    std::cout << std::string(70, '=') << "\n";
    
    LayoutConfig forceConfig;
    forceConfig.strategy = LayoutStrategy::Force;
    forceConfig.respectConnections = true;
    
    result = engine.arrangeElements(elements, forceConfig);
    std::cout << "Success: " << (result.success ? "Yes" : "No") << "\n";
    std::cout << "Elements arranged: " << result.elementsArranged << "\n";
    std::cout << "Total area used: " << result.totalArea << " sq units\n";
    
    printElementPositions(elements);

    overlaps = engine.countOverlaps(elements);
    std::cout << "Overlapping elements: " << overlaps << "\n";

    // Summary
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "SUMMARY\n";
    std::cout << std::string(70, '=') << "\n";
    std::cout << "The LayoutEngine successfully arranged " << result.elementsArranged 
             << " drawing elements\n";
    std::cout << "using different layout strategies while respecting relationships.\n";
    std::cout << "All elements are now positioned to create a clean and elegant UI.\n";

    return 0;
}
