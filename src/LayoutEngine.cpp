#include "LayoutEngine.h"
#include <cmath>
#include <algorithm>
#include <map>

namespace UFMTooling {

    // ============================================================================
    // LayoutEngine::Impl
    // ============================================================================
    class LayoutEngine::Impl {
    public:
        CanvasSize canvasSize;
        LayoutConfig config;

        Impl() : canvasSize() {}
        explicit Impl(const CanvasSize& size) : canvasSize(size) {}

        // Layout strategies
        LayoutResult arrangeGrid(std::vector<std::shared_ptr<ShapeElement>>& elements);
        LayoutResult arrangeHierarchical(std::vector<std::shared_ptr<ShapeElement>>& elements);
        LayoutResult arrangeForce(std::vector<std::shared_ptr<ShapeElement>>& elements);
        LayoutResult arrangeCircular(std::vector<std::shared_ptr<ShapeElement>>& elements);

        // Helper methods
        bool checkOverlap(const std::shared_ptr<ShapeElement>& elem1,
                         const std::shared_ptr<ShapeElement>& elem2) const;
        
        std::vector<std::shared_ptr<DrawingElement>> getDrawingElements(
            const std::vector<std::shared_ptr<ShapeElement>>& elements) const;
        
        std::vector<std::shared_ptr<RelationshipElement>> getRelationshipElements(
            const std::vector<std::shared_ptr<ShapeElement>>& elements) const;
    };

    // Helper: Get only drawing elements from the list
    std::vector<std::shared_ptr<DrawingElement>> LayoutEngine::Impl::getDrawingElements(
        const std::vector<std::shared_ptr<ShapeElement>>& elements) const {
        
        std::vector<std::shared_ptr<DrawingElement>> drawings;
        for (const auto& elem : elements) {
            if (elem && elem->getElementType() == ElementType::Drawing) {
                drawings.push_back(std::static_pointer_cast<DrawingElement>(elem));
            }
        }
        return drawings;
    }

    // Helper: Get only relationship elements from the list
    std::vector<std::shared_ptr<RelationshipElement>> LayoutEngine::Impl::getRelationshipElements(
        const std::vector<std::shared_ptr<ShapeElement>>& elements) const {
        
        std::vector<std::shared_ptr<RelationshipElement>> relationships;
        for (const auto& elem : elements) {
            if (elem && elem->getElementType() == ElementType::Relationship) {
                relationships.push_back(std::static_pointer_cast<RelationshipElement>(elem));
            }
        }
        return relationships;
    }

    // Check if two elements overlap
    bool LayoutEngine::Impl::checkOverlap(const std::shared_ptr<ShapeElement>& elem1,
                                          const std::shared_ptr<ShapeElement>& elem2) const {
        if (!elem1 || !elem2) return false;
        
        // Only check drawing elements (relationships don't occupy space)
        if (elem1->getElementType() != ElementType::Drawing || 
            elem2->getElementType() != ElementType::Drawing) {
            return false;
        }

        const Position& pos1 = elem1->getPosition();
        const Size& size1 = elem1->getSize();
        const Position& pos2 = elem2->getPosition();
        const Size& size2 = elem2->getSize();

        // Check if rectangles overlap (with padding)
        double padding = config.padding;
        
        bool overlapX = (pos1.x + size1.width + padding > pos2.x) && 
                       (pos2.x + size2.width + padding > pos1.x);
        bool overlapY = (pos1.y + size1.height + padding > pos2.y) && 
                       (pos2.y + size2.height + padding > pos1.y);

        return overlapX && overlapY;
    }

    // Grid layout strategy - arrange elements in a grid
    LayoutResult LayoutEngine::Impl::arrangeGrid(std::vector<std::shared_ptr<ShapeElement>>& elements) {
        LayoutResult result;
        
        // Get only drawing elements
        auto drawings = getDrawingElements(elements);
        
        if (drawings.empty()) {
            result.success = true;
            result.elementsArranged = 0;
            return result;
        }

        // Calculate available space
        double availableWidth = canvasSize.width - config.marginLeft - config.marginRight;
        double availableHeight = canvasSize.height - config.marginTop - config.marginBottom;

        // Find maximum element dimensions
        double maxWidth = 0.0;
        double maxHeight = 0.0;
        for (const auto& elem : drawings) {
            maxWidth = std::max(maxWidth, elem->getSize().width);
            maxHeight = std::max(maxHeight, elem->getSize().height);
        }

        // Calculate grid dimensions
        double cellWidth = maxWidth + config.padding;
        double cellHeight = maxHeight + config.padding;
        
        int cols = std::max(1, static_cast<int>(availableWidth / cellWidth));
        int rows = static_cast<int>(std::ceil(static_cast<double>(drawings.size()) / cols));

        // Check if grid fits in canvas
        if (rows * cellHeight > availableHeight) {
            // Adjust to fit vertically by increasing columns
            rows = std::max(1, static_cast<int>(availableHeight / cellHeight));
            cols = static_cast<int>(std::ceil(static_cast<double>(drawings.size()) / rows));
        }

        // Arrange elements in grid
        int index = 0;
        for (int row = 0; row < rows && index < static_cast<int>(drawings.size()); ++row) {
            for (int col = 0; col < cols && index < static_cast<int>(drawings.size()); ++col) {
                double x = config.marginLeft + col * cellWidth;
                double y = config.marginTop + row * cellHeight;
                
                drawings[index]->setPosition(x, y);
                ++index;
            }
        }

        result.success = true;
        result.elementsArranged = static_cast<int>(drawings.size());
        result.totalArea = availableWidth * availableHeight;

        return result;
    }

    // Hierarchical layout - arrange elements based on relationships
    LayoutResult LayoutEngine::Impl::arrangeHierarchical(
        std::vector<std::shared_ptr<ShapeElement>>& elements) {
        
        LayoutResult result;
        
        auto drawings = getDrawingElements(elements);
        auto relationships = getRelationshipElements(elements);

        if (drawings.empty()) {
            result.success = true;
            result.elementsArranged = 0;
            return result;
        }

        // Build a simple parent-child relationship map
        std::map<std::shared_ptr<DrawingElement>, std::vector<std::shared_ptr<DrawingElement>>> childMap;
        std::map<std::shared_ptr<DrawingElement>, int> levelMap;
        
        // Find root elements (elements with no incoming relationships)
        std::vector<std::shared_ptr<DrawingElement>> roots;
        for (const auto& elem : drawings) {
            bool hasParent = false;
            for (const auto& rel : relationships) {
                if (rel->getConnector2() == elem) {
                    hasParent = true;
                    break;
                }
            }
            if (!hasParent) {
                roots.push_back(elem);
                levelMap[elem] = 0;
            }
        }

        // If no clear hierarchy, treat all as roots
        if (roots.empty()) {
            roots = drawings;
            for (const auto& elem : roots) {
                levelMap[elem] = 0;
            }
        }

        // Build child map and assign levels
        for (const auto& rel : relationships) {
            auto parent = rel->getConnector1();
            auto child = rel->getConnector2();
            if (parent && child) {
                childMap[parent].push_back(child);
                if (levelMap.find(parent) != levelMap.end()) {
                    levelMap[child] = levelMap[parent] + 1;
                }
            }
        }

        // Calculate available space
        double availableWidth = canvasSize.width - config.marginLeft - config.marginRight;
        double verticalSpacing = 150.0; // Fixed vertical spacing between levels

        // Find number of levels
        int maxLevel = 0;
        for (const auto& pair : levelMap) {
            maxLevel = std::max(maxLevel, pair.second);
        }

        // Arrange elements by level
        std::vector<std::vector<std::shared_ptr<DrawingElement>>> levels(maxLevel + 1);
        for (const auto& elem : drawings) {
            int level = (levelMap.find(elem) != levelMap.end()) ? levelMap[elem] : 0;
            levels[level].push_back(elem);
        }

        // Position elements
        for (int level = 0; level <= maxLevel; ++level) {
            int count = static_cast<int>(levels[level].size());
            if (count == 0) continue;

            double horizontalSpacing = availableWidth / (count + 1);
            double y = config.marginTop + level * verticalSpacing;

            for (int i = 0; i < count; ++i) {
                double x = config.marginLeft + (i + 1) * horizontalSpacing - 
                          levels[level][i]->getSize().width / 2;
                levels[level][i]->setPosition(x, y);
            }
        }

        result.success = true;
        result.elementsArranged = static_cast<int>(drawings.size());
        result.totalArea = availableWidth * (canvasSize.height - config.marginTop - config.marginBottom);

        return result;
    }

    // Force-directed layout - use physical simulation
    LayoutResult LayoutEngine::Impl::arrangeForce(
        std::vector<std::shared_ptr<ShapeElement>>& elements) {
        
        LayoutResult result;
        
        auto drawings = getDrawingElements(elements);
        auto relationships = getRelationshipElements(elements);

        if (drawings.empty()) {
            result.success = true;
            result.elementsArranged = 0;
            return result;
        }

        // Initialize positions randomly
        double centerX = canvasSize.width / 2;
        double centerY = canvasSize.height / 2;
        double spread = 200.0;

        for (size_t i = 0; i < drawings.size(); ++i) {
            double angle = (2.0 * M_PI * i) / drawings.size();
            double x = centerX + spread * std::cos(angle);
            double y = centerY + spread * std::sin(angle);
            drawings[i]->setPosition(x, y);
        }

        // Force-directed iterations
        const int iterations = 50;
        const double k = 100.0; // Spring constant
        const double repulsion = 5000.0; // Repulsion constant

        for (int iter = 0; iter < iterations; ++iter) {
            // Calculate forces for each element
            std::vector<Position> forces(drawings.size(), Position(0.0, 0.0));

            // Repulsion between all nodes
            for (size_t i = 0; i < drawings.size(); ++i) {
                for (size_t j = i + 1; j < drawings.size(); ++j) {
                    Position pos1 = drawings[i]->getPosition();
                    Position pos2 = drawings[j]->getPosition();
                    
                    double dx = pos1.x - pos2.x;
                    double dy = pos1.y - pos2.y;
                    double dist = std::sqrt(dx * dx + dy * dy);
                    
                    if (dist < 1.0) dist = 1.0; // Avoid division by zero
                    
                    double force = repulsion / (dist * dist);
                    forces[i].x += force * dx / dist;
                    forces[i].y += force * dy / dist;
                    forces[j].x -= force * dx / dist;
                    forces[j].y -= force * dy / dist;
                }
            }

            // Attraction along relationships
            for (const auto& rel : relationships) {
                auto conn1 = rel->getConnector1();
                auto conn2 = rel->getConnector2();
                if (!conn1 || !conn2) continue;

                // Find indices
                auto it1 = std::find(drawings.begin(), drawings.end(), conn1);
                auto it2 = std::find(drawings.begin(), drawings.end(), conn2);
                if (it1 == drawings.end() || it2 == drawings.end()) continue;

                size_t i = std::distance(drawings.begin(), it1);
                size_t j = std::distance(drawings.begin(), it2);

                Position pos1 = conn1->getPosition();
                Position pos2 = conn2->getPosition();
                
                double dx = pos2.x - pos1.x;
                double dy = pos2.y - pos1.y;
                double dist = std::sqrt(dx * dx + dy * dy);
                
                if (dist < 1.0) dist = 1.0;
                
                double force = k * (dist - 200.0) / dist; // Target distance of 200
                forces[i].x += force * dx / dist;
                forces[i].y += force * dy / dist;
                forces[j].x -= force * dx / dist;
                forces[j].y -= force * dy / dist;
            }

            // Apply forces
            double damping = 0.8;
            for (size_t i = 0; i < drawings.size(); ++i) {
                Position pos = drawings[i]->getPosition();
                pos.x += forces[i].x * damping;
                pos.y += forces[i].y * damping;

                // Keep within bounds
                pos.x = std::max(config.marginLeft, 
                    std::min(pos.x, canvasSize.width - config.marginRight - drawings[i]->getSize().width));
                pos.y = std::max(config.marginTop, 
                    std::min(pos.y, canvasSize.height - config.marginBottom - drawings[i]->getSize().height));

                drawings[i]->setPosition(pos);
            }
        }

        result.success = true;
        result.elementsArranged = static_cast<int>(drawings.size());
        result.totalArea = canvasSize.width * canvasSize.height;

        return result;
    }

    // Circular layout - arrange elements in a circle
    LayoutResult LayoutEngine::Impl::arrangeCircular(
        std::vector<std::shared_ptr<ShapeElement>>& elements) {
        
        LayoutResult result;
        
        auto drawings = getDrawingElements(elements);

        if (drawings.empty()) {
            result.success = true;
            result.elementsArranged = 0;
            return result;
        }

        // Calculate center and radius
        double centerX = canvasSize.width / 2;
        double centerY = canvasSize.height / 2;
        double availableWidth = canvasSize.width - config.marginLeft - config.marginRight;
        double availableHeight = canvasSize.height - config.marginTop - config.marginBottom;
        double radius = std::min(availableWidth, availableHeight) / 2 - 100.0;

        // Arrange in circle
        for (size_t i = 0; i < drawings.size(); ++i) {
            double angle = (2.0 * M_PI * i) / drawings.size();
            double x = centerX + radius * std::cos(angle) - drawings[i]->getSize().width / 2;
            double y = centerY + radius * std::sin(angle) - drawings[i]->getSize().height / 2;
            drawings[i]->setPosition(x, y);
        }

        result.success = true;
        result.elementsArranged = static_cast<int>(drawings.size());
        result.totalArea = M_PI * radius * radius;

        return result;
    }

    // ============================================================================
    // LayoutEngine
    // ============================================================================
    LayoutEngine::LayoutEngine() : pImpl(std::make_unique<Impl>()) {
    }

    LayoutEngine::LayoutEngine(const CanvasSize& canvasSize) 
        : pImpl(std::make_unique<Impl>(canvasSize)) {
    }

    LayoutEngine::~LayoutEngine() = default;

    LayoutResult LayoutEngine::arrangeElements(std::vector<std::shared_ptr<ShapeElement>>& elements) {
        return arrangeElements(elements, pImpl->config);
    }

    LayoutResult LayoutEngine::arrangeElements(std::vector<std::shared_ptr<ShapeElement>>& elements,
                                              const LayoutConfig& config) {
        pImpl->config = config;

        // Choose layout strategy
        switch (config.strategy) {
            case LayoutStrategy::Grid:
                return pImpl->arrangeGrid(elements);
            case LayoutStrategy::Hierarchical:
                return pImpl->arrangeHierarchical(elements);
            case LayoutStrategy::Force:
                return pImpl->arrangeForce(elements);
            case LayoutStrategy::Circular:
                return pImpl->arrangeCircular(elements);
            default:
                return pImpl->arrangeGrid(elements);
        }
    }

    void LayoutEngine::setCanvasSize(const CanvasSize& size) {
        pImpl->canvasSize = size;
    }

    void LayoutEngine::setCanvasSize(double width, double height) {
        pImpl->canvasSize.width = width;
        pImpl->canvasSize.height = height;
    }

    const CanvasSize& LayoutEngine::getCanvasSize() const {
        return pImpl->canvasSize;
    }

    void LayoutEngine::setLayoutConfig(const LayoutConfig& config) {
        pImpl->config = config;
    }

    const LayoutConfig& LayoutEngine::getLayoutConfig() const {
        return pImpl->config;
    }

    void LayoutEngine::setLayoutStrategy(LayoutStrategy strategy) {
        pImpl->config.strategy = strategy;
    }

    LayoutStrategy LayoutEngine::getLayoutStrategy() const {
        return pImpl->config.strategy;
    }

    bool LayoutEngine::checkOverlap(const std::shared_ptr<ShapeElement>& elem1,
                                    const std::shared_ptr<ShapeElement>& elem2) const {
        return pImpl->checkOverlap(elem1, elem2);
    }

    int LayoutEngine::countOverlaps(const std::vector<std::shared_ptr<ShapeElement>>& elements) const {
        int count = 0;
        for (size_t i = 0; i < elements.size(); ++i) {
            for (size_t j = i + 1; j < elements.size(); ++j) {
                if (checkOverlap(elements[i], elements[j])) {
                    ++count;
                }
            }
        }
        return count;
    }

} // namespace UFMTooling
