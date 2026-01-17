#ifndef LAYOUT_ENGINE_H
#define LAYOUT_ENGINE_H

#include "ShapeElement.h"
#include <vector>
#include <memory>

namespace UFMTooling {

    // Structure to represent screen/canvas dimensions
    struct CanvasSize {
        double width;
        double height;

        CanvasSize() : width(1920.0), height(1080.0) {}
        CanvasSize(double w, double h) : width(w), height(h) {}
    };

    // Enum for layout strategies
    enum class LayoutStrategy {
        Grid,           // Arrange elements in a grid pattern
        Hierarchical,   // Arrange elements in a hierarchical tree-like structure
        Force,          // Use force-directed layout for natural spacing
        Circular        // Arrange elements in a circular pattern
    };

    // Structure for layout configuration
    struct LayoutConfig {
        LayoutStrategy strategy;
        double padding;           // Minimum space between elements
        double marginTop;         // Top margin
        double marginBottom;      // Bottom margin
        double marginLeft;        // Left margin
        double marginRight;       // Right margin
        bool respectConnections;  // Consider relationships when positioning

        LayoutConfig() 
            : strategy(LayoutStrategy::Grid),
              padding(20.0),
              marginTop(50.0),
              marginBottom(50.0),
              marginLeft(50.0),
              marginRight(50.0),
              respectConnections(true) {}
    };

    // Result structure for layout operations
    struct LayoutResult {
        bool success;
        std::string errorMessage;
        int elementsArranged;
        double totalArea;

        LayoutResult() : success(false), elementsArranged(0), totalArea(0.0) {}
    };

    // Main layout engine class
    class LayoutEngine {
    public:
        LayoutEngine();
        explicit LayoutEngine(const CanvasSize& canvasSize);
        ~LayoutEngine();

        // Main layout method
        LayoutResult arrangeElements(std::vector<std::shared_ptr<ShapeElement>>& elements);
        LayoutResult arrangeElements(std::vector<std::shared_ptr<ShapeElement>>& elements, 
                                    const LayoutConfig& config);

        // Configuration methods
        void setCanvasSize(const CanvasSize& size);
        void setCanvasSize(double width, double height);
        const CanvasSize& getCanvasSize() const;

        void setLayoutConfig(const LayoutConfig& config);
        const LayoutConfig& getLayoutConfig() const;

        // Layout strategy methods
        void setLayoutStrategy(LayoutStrategy strategy);
        LayoutStrategy getLayoutStrategy() const;

        // Utility methods
        bool checkOverlap(const std::shared_ptr<ShapeElement>& elem1,
                         const std::shared_ptr<ShapeElement>& elem2) const;
        
        int countOverlaps(const std::vector<std::shared_ptr<ShapeElement>>& elements) const;

    private:
        class Impl;
        std::unique_ptr<Impl> pImpl;
    };

} // namespace UFMTooling

#endif // LAYOUT_ENGINE_H
