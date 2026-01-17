#ifndef SAMPLE_HEADER_H
#define SAMPLE_HEADER_H

#include <string>
#include <vector>
#include <memory>

namespace Graphics {

    // Forward declaration
    class Renderer;

    // Enum for colors
    enum class Color {
        Red,
        Green,
        Blue,
        Yellow,
        Black,
        White
    };

    // Base shape class
    class Shape {
    public:
        Shape();
        virtual ~Shape();
        
        // Pure virtual methods
        virtual double calculateArea() const = 0;
        virtual double calculatePerimeter() const = 0;
        virtual void draw(Renderer* renderer) const = 0;
        
        // Regular methods
        void setColor(Color color);
        Color getColor() const;
        void setPosition(double x, double y);
        std::pair<double, double> getPosition() const;
        
        // Static method
        static int getShapeCount();
        
    protected:
        double m_x;
        double m_y;
        Color m_color;
        
    private:
        static int s_shapeCount;
        bool m_visible;
    };

    // Circle class
    class Circle : public Shape {
    public:
        Circle(double radius);
        Circle(double x, double y, double radius);
        virtual ~Circle();
        
        virtual double calculateArea() const override;
        virtual double calculatePerimeter() const override;
        virtual void draw(Renderer* renderer) const override;
        
        void setRadius(double radius);
        double getRadius() const;
        
    private:
        double m_radius;
    };

    // Rectangle class
    class Rectangle : public Shape {
    public:
        Rectangle(double width, double height);
        Rectangle(double x, double y, double width, double height);
        virtual ~Rectangle();
        
        virtual double calculateArea() const override;
        virtual double calculatePerimeter() const override;
        virtual void draw(Renderer* renderer) const override;
        
        void setWidth(double width);
        void setHeight(double height);
        double getWidth() const;
        double getHeight() const;
        
    private:
        double m_width;
        double m_height;
    };

    // Triangle class
    class Triangle : public Shape {
    public:
        Triangle(double sideA, double sideB, double sideC);
        virtual ~Triangle();
        
        virtual double calculateArea() const override;
        virtual double calculatePerimeter() const override;
        virtual void draw(Renderer* renderer) const override;
        
        bool isValid() const;
        
    private:
        double m_sideA;
        double m_sideB;
        double m_sideC;
    };

    // Point structure
    struct Point2D {
        double x;
        double y;
        
        Point2D() : x(0), y(0) {}
        Point2D(double px, double py) : x(px), y(py) {}
        
        double distanceTo(const Point2D& other) const;
    };

    // Vector structure
    struct Vector2D {
        double x;
        double y;
        
        Vector2D() : x(0), y(0) {}
        Vector2D(double vx, double vy) : x(vx), y(vy) {}
        
        double length() const;
        void normalize();
        Vector2D operator+(const Vector2D& other) const;
        Vector2D operator-(const Vector2D& other) const;
        Vector2D operator*(double scalar) const;
    };

    // Renderer interface
    class Renderer {
    public:
        virtual ~Renderer() = default;
        
        virtual void drawCircle(double x, double y, double radius, Color color) = 0;
        virtual void drawRectangle(double x, double y, double width, double height, Color color) = 0;
        virtual void drawLine(double x1, double y1, double x2, double y2, Color color) = 0;
        virtual void clear() = 0;
    };

    // Canvas class
    class Canvas {
    public:
        Canvas(int width, int height);
        ~Canvas();
        
        void addShape(std::shared_ptr<Shape> shape);
        void removeShape(std::shared_ptr<Shape> shape);
        void clear();
        void render(Renderer* renderer);
        
        int getWidth() const { return m_width; }
        int getHeight() const { return m_height; }
        
    private:
        int m_width;
        int m_height;
        std::vector<std::shared_ptr<Shape>> m_shapes;
    };

} // namespace Graphics

#endif // SAMPLE_HEADER_H
