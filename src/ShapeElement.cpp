#include "ShapeElement.h"

namespace UFMTooling {

    // ============================================================================
    // ShapeElement::Impl
    // ============================================================================
    class ShapeElement::Impl {
    public:
        Position position;
        Size size;
        std::string id;

        Impl() {}
    };

    // ============================================================================
    // ShapeElement
    // ============================================================================
    ShapeElement::ShapeElement() : pImpl(std::make_unique<Impl>()) {
    }

    ShapeElement::~ShapeElement() = default;

    const Position& ShapeElement::getPosition() const {
        return pImpl->position;
    }

    const Size& ShapeElement::getSize() const {
        return pImpl->size;
    }

    const std::string& ShapeElement::getId() const {
        return pImpl->id;
    }

    void ShapeElement::setPosition(const Position& pos) {
        pImpl->position = pos;
    }

    void ShapeElement::setPosition(double x, double y) {
        pImpl->position.x = x;
        pImpl->position.y = y;
    }

    void ShapeElement::setSize(const Size& size) {
        pImpl->size = size;
    }

    void ShapeElement::setSize(double width, double height) {
        pImpl->size.width = width;
        pImpl->size.height = height;
    }

    void ShapeElement::setId(const std::string& id) {
        pImpl->id = id;
    }

    // ============================================================================
    // DrawingElement::Impl
    // ============================================================================
    class DrawingElement::Impl {
    public:
        std::string name;
        std::string shapeType;
        std::string color;

        Impl() : shapeType("rectangle"), color("#FFFFFF") {}
    };

    // ============================================================================
    // DrawingElement
    // ============================================================================
    DrawingElement::DrawingElement() : ShapeElement(), pImpl(std::make_unique<Impl>()) {
        // Set default size for drawing elements
        setSize(100.0, 60.0);
    }

    DrawingElement::DrawingElement(const std::string& name) 
        : ShapeElement(), pImpl(std::make_unique<Impl>()) {
        pImpl->name = name;
        setSize(100.0, 60.0);
    }

    DrawingElement::~DrawingElement() = default;

    const std::string& DrawingElement::getName() const {
        return pImpl->name;
    }

    const std::string& DrawingElement::getShapeType() const {
        return pImpl->shapeType;
    }

    const std::string& DrawingElement::getColor() const {
        return pImpl->color;
    }

    void DrawingElement::setName(const std::string& name) {
        pImpl->name = name;
    }

    void DrawingElement::setShapeType(const std::string& shapeType) {
        pImpl->shapeType = shapeType;
    }

    void DrawingElement::setColor(const std::string& color) {
        pImpl->color = color;
    }

    ElementType DrawingElement::getElementType() const {
        return ElementType::Drawing;
    }

    // ============================================================================
    // RelationshipElement::Impl
    // ============================================================================
    class RelationshipElement::Impl {
    public:
        std::shared_ptr<DrawingElement> connector1;
        std::shared_ptr<DrawingElement> connector2;
        std::string relationshipType;
        std::string label;

        Impl() : relationshipType("association") {}
    };

    // ============================================================================
    // RelationshipElement
    // ============================================================================
    RelationshipElement::RelationshipElement() 
        : ShapeElement(), pImpl(std::make_unique<Impl>()) {
        // Relationships don't have fixed size, they're rendered as lines
        setSize(0.0, 0.0);
    }

    RelationshipElement::RelationshipElement(std::shared_ptr<DrawingElement> conn1,
                                            std::shared_ptr<DrawingElement> conn2)
        : ShapeElement(), pImpl(std::make_unique<Impl>()) {
        pImpl->connector1 = conn1;
        pImpl->connector2 = conn2;
        setSize(0.0, 0.0);
    }

    RelationshipElement::~RelationshipElement() = default;

    std::shared_ptr<DrawingElement> RelationshipElement::getConnector1() const {
        return pImpl->connector1;
    }

    std::shared_ptr<DrawingElement> RelationshipElement::getConnector2() const {
        return pImpl->connector2;
    }

    const std::string& RelationshipElement::getRelationshipType() const {
        return pImpl->relationshipType;
    }

    const std::string& RelationshipElement::getLabel() const {
        return pImpl->label;
    }

    void RelationshipElement::setConnector1(std::shared_ptr<DrawingElement> connector) {
        pImpl->connector1 = connector;
    }

    void RelationshipElement::setConnector2(std::shared_ptr<DrawingElement> connector) {
        pImpl->connector2 = connector;
    }

    void RelationshipElement::setRelationshipType(const std::string& type) {
        pImpl->relationshipType = type;
    }

    void RelationshipElement::setLabel(const std::string& label) {
        pImpl->label = label;
    }

    ElementType RelationshipElement::getElementType() const {
        return ElementType::Relationship;
    }

} // namespace UFMTooling
