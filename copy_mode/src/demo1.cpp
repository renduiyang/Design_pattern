//
// Created by renduiyang on 24-10-7.
//
#include <iostream>
#include <unordered_map>

//1. 抽象原型类
class Shape {
public:
    virtual ~Shape() {
    }

    virtual Shape *clone() = 0;

    virtual int getid() = 0;

    virtual std::string getType() = 0;

protected:
    std::string Type;

private:
    int id;
};

//2. 三个形状具体原型
class Circle : public Shape {
public:
    Circle(std::string Type, int id) : Type(Type), id(id) {
    }

    ~Circle() {
    }

    //Circle(const Circle& lhs) { Type = lhs.Type, id = lhs.id; }
    Shape *clone() { return new Circle{*this}; }
    int getid() { return id; }
    std::string getType() { return Type; }

protected:
    std::string Type;

private:
    int id;
};

class Rectangle : public Shape {
public:
    Rectangle(std::string Type, int id) : Type(Type), id(id) {
    }

    ~Rectangle() {
    }

    Rectangle(const Rectangle &lhs) { Type = lhs.Type, id = lhs.id; }
    Shape *clone() { return new Rectangle{*this}; }
    int getid() { return id; }
    std::string getType() { return Type; }

protected:
    std::string Type;

private:
    int id;
};

class Square : public Shape {
public:
    Square(std::string Type, int id) : Type(Type), id(id) {
    }

    ~Square() {
    }

    Square(const Square &lhs) { Type = lhs.Type, id = lhs.id; }
    Shape *clone() { return new Square{*this}; }
    int getid() { return id; }
    std::string getType() { return Type; }

protected:
    std::string Type;

private:
    int id;
};

//3. 存储对象种类的数据库
class ShapeType {
public:
    ~ShapeType() {
        for (auto &x: ShapeMap) {
            delete x.second;
            x.second = nullptr;
        }
    }

    //构造原始对象
    ShapeType() {
        Circle *circle = new Circle{"circular", 1};
        Square *square = new Square{"square", 2};
        Rectangle *rectangle = new Rectangle{"rectangular", 3};
        ShapeMap.emplace(circle->getType(), circle);
        ShapeMap.emplace(square->getType(), square);
        ShapeMap.emplace(rectangle->getType(), rectangle);
    }

    //根据你所需要的种类来获得克隆对象
    Shape *getShape(std::string Type) {
        return ShapeMap[Type]->clone();
    }

private:
    std::unordered_map<std::string, Shape *> ShapeMap;
};

int main() {
    //1. 创建对象种类库
    ShapeType obj;

    //2. 从对象库中获得对象的克隆体（getShape函数返回某个对象的克隆）
    Shape *m_circle = obj.getShape("circular");
    Shape *m_Square = obj.getShape("square");
    Shape *m_Rectangle = obj.getShape("rectangular");



    std::cout << m_circle->getid() << " : " << m_circle->getType() << std::endl;
    std::cout << m_Square->getid() << " : " << m_Square->getType() << std::endl;
    std::cout << m_Rectangle->getid() << " : " << m_Rectangle->getType() << std::endl;


    delete m_circle;
    delete m_Square;
    delete m_Rectangle;
    return 0;
}
