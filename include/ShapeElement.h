#ifndef SHAPE_ELEMENT_H
#define SHAPE_ELEMENT_H

#include <string>
#include <memory>

namespace UFMTooling {

    // Enum for element types
    enum class ElementType {
        Drawing,
        Relationship
    };

    // Structure to represent a 2D position
    struct Position {
        double x;
        double y;

        Position() : x(0.0), y(0.0) {}
        Position(double x_, double y_) : x(x_), y(y_) {}
    };

    // Structure to represent dimensions
    struct Size {
        double width;
        double height;

        Size() : width(0.0), height(0.0) {}
        Size(double w, double h) : width(w), height(h) {}
    };

    // Base class for all shape elements
    class ShapeElement {
    public:
        ShapeElement();
        virtual ~ShapeElement();

        // Getters
        ElementType getType() const;
        const Position& getPosition() const;
        const Size& getSize() const;
        const std::string& getId() const;

        // Setters
        void setPosition(const Position& pos);
        void setPosition(double x, double y);
        void setSize(const Size& size);
        void setSize(double width, double height);
        void setId(const std::string& id);

        // Virtual method to get element type (overridden in derived classes)
        virtual ElementType getElementType() const = 0;

    protected:
        void setType(ElementType type);

    private:
        class Impl;
        std::unique_ptr<Impl> pImpl;
    };

    // Drawing element class
    class DrawingElement : public ShapeElement {
    public:
        DrawingElement();
        DrawingElement(const std::string& name);
        virtual ~DrawingElement();

        // Getters
        const std::string& getName() const;
        const std::string& getShapeType() const;
        const std::string& getColor() const;

        // Setters
        void setName(const std::string& name);
        void setShapeType(const std::string& shapeType);
        void setColor(const std::string& color);

        // Override from base class
        ElementType getElementType() const override;

    private:
        class Impl;
        std::unique_ptr<Impl> pImpl;
    };

    // Relationship element class
    class RelationshipElement : public ShapeElement {
    public:
        RelationshipElement();
        RelationshipElement(std::shared_ptr<DrawingElement> conn1, 
                           std::shared_ptr<DrawingElement> conn2);
        virtual ~RelationshipElement();

        // Getters
        std::shared_ptr<DrawingElement> getConnector1() const;
        std::shared_ptr<DrawingElement> getConnector2() const;
        const std::string& getRelationshipType() const;
        const std::string& getLabel() const;

        // Setters
        void setConnector1(std::shared_ptr<DrawingElement> connector);
        void setConnector2(std::shared_ptr<DrawingElement> connector);
        void setRelationshipType(const std::string& type);
        void setLabel(const std::string& label);

        // Override from base class
        ElementType getElementType() const override;

    private:
        class Impl;
        std::unique_ptr<Impl> pImpl;
    };

} // namespace UFMTooling

#endif // SHAPE_ELEMENT_H
