//
// Created by renduiyang on 24-10-6.
//
#include<iostream>
#include<memory>

struct Point2D {
    int x;
    int y;
};

enum class Material {
    brick,
    aerated_concrete,
    drywall,
};

enum class WallType {
    basic,
    main,
    partition,
};

class Wall {
    Point2D start; // 墙的起始点
    Point2D end; // 墙的终止点
    int elevation; // 墙的海拔，相对于某个基线的高度
    int height; // 墙的高度
public:
    Wall(Point2D start, Point2D end, int elevation, int height)
        : start(start), end{end}, elevation{elevation}, height{height} {
    }

    // 判断墙体是否相交
    bool intersects(const Wall &wall) {
        return false;
    }

    virtual void print(std::ostream &os) const {
        os << "start: " << start.x << ", " << start.y
                << "\nend: " << end.x << ", " << end.y
                << "\nelevation: " << elevation
                << "\nheight: " << height;
    }

    friend std::ostream &operator<<(std::ostream &os, const Wall &wall) {
        wall.print(os);
        return os;
    }
};



class SoildWall : public Wall {
    friend class WallFactory;
    int width; //墙的宽度
    Material material; // 墙的材质
protected:
    SoildWall(Point2D start, Point2D end, int elevation, int height,
              int width, Material material)
        : Wall{start, end, elevation, height}, width{width}, material{material} {
    }

public:
    void print(std::ostream &os) const override {
        Wall::print(os); // 调用基类的输出函数
        os << "\nwidth: " << width
                << "\nmaterial: " << static_cast<int>(material);
    }

    friend std::ostream &operator<<(std::ostream &os, const SoildWall &wall) {
        wall.print(os);
        return os;
    }

    static SoildWall create_main(Point2D start, Point2D end, int elevation, int height) {
        return SoildWall(start, end, elevation, height, 375, Material::aerated_concrete);
    }

    static std::unique_ptr<SoildWall> create_partition(Point2D start,
                                                       Point2D end, int elevation, int height) {
        return std::make_unique<SoildWall>(SoildWall(start, end, elevation, height, 120, Material::brick));
    }

    static std::shared_ptr<SoildWall> create_main2(Point2D start,
                                                   Point2D end, int elevation, int height) {
        if (elevation < 0) {
            return {};
        }
        return std::make_shared<SoildWall>(SoildWall(start,
                                                     end, elevation, height, 120, Material::brick));
    }
};

void testFactoryMethod() {
    Point2D start{0, 0}, end{0, 3000};
    const auto main_wall = SoildWall::create_main({0, 0}, {0, 3000}, 2700, 3000);
    std::cout << main_wall << "\n";
}

void testFactoryCreateMain2() {
    const auto also_main_wall = SoildWall::create_main2({0, 0}, {0, 3000}, -2000, 3000);
    if (!also_main_wall) {
        std::cout << "main wall create failed." << std::endl;
    }
}

// 3.3 工厂
#include<vector>

class WallFactory {
private:
    static std::vector<std::weak_ptr<Wall> > walls;

public:
    static std::shared_ptr<SoildWall> create_mian(Point2D start,
                                                  Point2D end, int elevation, int height) {
        const auto this_wall = new SoildWall(start, end, elevation,
                                             height, 120, Material::brick);

        for (const auto wall: walls) {
            if (auto p = wall.lock()) {
                if (this_wall->intersects(*p)) {
                    delete this_wall;
                    return {};
                }
            }
        }
        std::shared_ptr<SoildWall> ptr(this_wall);
        walls.push_back(ptr);
        return ptr;
    }

    // 3.4
    static std::shared_ptr<Wall> create_wall(WallType type, Point2D start,
                                             Point2D end, int elevation, int height) {
        switch (type) {
            case WallType::main: {
                return create_mian(start, end, elevation, height);
                // return std::make_shared<SoildWall>(SoildWall(start, end, elevation, height,
                                                             // 375, Material::aerated_concrete));
            }

            case WallType::partition: {
                return std::make_shared<SoildWall>(SoildWall(start, end, elevation, height,
                                                             120, Material::brick));
            }

            case WallType::basic: {
                // return std::make_shared<Wall>(start, end, elevation, height);
                return std::shared_ptr<Wall>{new Wall(start, end, elevation, height)};
            }
            default:
                return {};
        }
    }
};

// 类外实例化
std::vector<std::weak_ptr<Wall> > WallFactory::walls;


void testWallFactory() {
    const auto partition = WallFactory::create_mian({2000, 0}, {2000, 4000}, 0, 2700);
    std::cout << *partition << std::endl;
}

// 3.4
void testPolymorphicFactory() {
    auto also_partition =
            WallFactory::create_wall(WallType::main, {0, 0}, {5000, 0}, 0, 4200);

    // 把WallType::partition 改为WallType::basic, 下面的tmp会为空，Segmentation fault（空指针异常）
    if (also_partition) {
        auto tmp = std::dynamic_pointer_cast<SoildWall>(also_partition);
        if (tmp == nullptr) {
            std::cout << "hahhahahha" << std::endl;
        }
        std::cout << "\ntestPolymorphicFactory: \n" <<
                *std::dynamic_pointer_cast<SoildWall>(also_partition) << "\n";
    }

    std::cout << "\ntestPolymorphicFactory2: \n" <<
            (*also_partition) << "\n";
}


int main() {
    testWallFactory();
    testPolymorphicFactory();

    return 0;
}
