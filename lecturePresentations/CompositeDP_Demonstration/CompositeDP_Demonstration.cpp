#include <iostream>
#include <vector>
#include <memory>

// Общий интерфейс
class Graphic {
public:
    virtual void move(int x, int y) = 0;
    virtual void draw() = 0;
    virtual ~Graphic() {}
};

// Примитивный объект — точка
class Dot : public Graphic {
protected:
    int x, y;
public:
    Dot(int x, int y) : x(x), y(y) {}
    void move(int dx, int dy) override {
        x += dx;
        y += dy;
    }
    void draw() override {
        std::cout << "Draw Dot at (" << x << ", " << y << ")\n";
    }
};

// Круг — наследник точки
class Circle : public Dot {
    int radius;
public:
    Circle(int x, int y, int r) : Dot(x, y), radius(r) {}
    void draw() override {
        std::cout << "Draw Circle at (" << x << ", " << y << ") with radius " << radius << "\n";
    }
};

// Композит — контейнер графических объектов
class CompoundGraphic : public Graphic {
    std::vector<std::shared_ptr<Graphic>> children;
public:
    void add(std::shared_ptr<Graphic> child) {
        children.push_back(child);
    }
    void remove(std::shared_ptr<Graphic> child) {
        children.erase(std::remove(children.begin(), children.end(), child), children.end());
    }
    void move(int dx, int dy) override {
        for (auto& child : children) {
            child->move(dx, dy);
        }
    }
    void draw() override {
        std::cout << "Draw CompoundGraphic:\n";
        for (auto& child : children) {
            child->draw();
        }
    }
};

// Клиент — графический редактор
class ImageEditor {
    std::shared_ptr<CompoundGraphic> all;
public:
    ImageEditor() : all(std::make_shared<CompoundGraphic>()) {}

    void load() {
        all->add(std::make_shared<Dot>(1, 2));
        all->add(std::make_shared<Circle>(3, 4, 5));
    }

    void render() {
        all->draw();
    }
};

int main() {
    ImageEditor editor;
    editor.load();
    editor.render();
    return 0;
}
