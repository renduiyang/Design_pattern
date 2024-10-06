//
// Created by renduiyang on 24-10-4.
//
#include <iostream>
#include <memory>
#include<ostream>
#include <vector>

struct point2D {
    double x, y;
};

class wall {
protected:
    point2D start, end;
    int elevation;
    int height;

public:
    wall(point2D start, point2D end, int elevation, int height) : start(start), end(end), elevation(elevation),
                                                                  height(height) {
    }

    virtual ~wall() {
        std::cout << "destruct wall\n";
    }

    bool intersects(const wall &other) const {
        return false;
    }

    friend std::ostream &operator <<(std::ostream &os, const wall &w) {
        return os << "wall from " << "(" << w.start.x << w.start.y << ")" << " to " << "(" << w.start.x << w.start.y <<
               ")" << " with elevation " << w.elevation << " and height "
               << w.height << "\n";
    }
};

/**
 * @brief enum class 表示访问时是私有类型 只能使用::访问
 * @note  使用enum 表示是公有类型 可以直接使用<<访问
 */
enum class material {
    brick_wall,
    gas_wall,
    drywall
};

/**
 * @brief solidwall 是一个 wall 的子类，类似现实世界中的墙的抽象化
 * @note  此处将构造函数设置为protected ，使其无法被外部实例化(因为墙在构造时有一定限制)
 * @note  这里的static方法即为工厂方法
 */
class solidWall : public wall {
    // 这里声明友元类 希望可以在wallFactor中使用solidWall的构造函数
    friend class wallFactory;
    int width;
    material material_;

protected:
    solidWall(point2D start, point2D end, int elevation, int height, int width, material material_): wall(start, end,
            elevation, height),
        width(width), material_(material_) {
    }

public:
    static solidWall creat_main(point2D start, point2D end, int elevation, int height) {
        return solidWall(start, end, elevation, height, 375, material::gas_wall);
    }

    ~solidWall() {
        std::cout << "destruct solid wall\n";
    }

    /**
     * @brief
     * @param start 起点坐标
     * @param end 终点坐标
     * @param elevation 海拔
     * @param height 高度
     * @return std::unique_ptr 返回指向solidWall的unique_ptr指针
     * @note make_unique can not use protected constructor
     */
    static std::unique_ptr<solidWall> creat_partition(point2D start, point2D end, int elevation, int height) {
        return std::make_unique<solidWall>(solidWall(start, end, elevation, height, 120, material::brick_wall));
        // return std::make_unique<solidWall>(start, end, elevation, height, 120, material::brick_wall);
    }

    friend std::ostream &operator<<(std::ostream &os, const solidWall &w) {
        std::string_view material_name;
        if (w.material_ == material::brick_wall) {
            material_name = "brick wall";
        } else if (w.material_ == material::gas_wall) {
            material_name = "gas wall";
        } else if (w.material_ == material::drywall) {
            material_name = "dry wall";
        } else {
            material_name = "unknown material";
        }
        return os << "wall from " << "(" << w.start.x << " , " << w.start.y << ")" << " to " << "(" << w.start.x <<
               " , " << w.start.y <<
               ")" << " with elevation " << w.elevation << " and height "
               << w.height << " and width "
               << w.width << " and material is " << material_name << "\n";
    }

    static std::shared_ptr<solidWall> create_main2(point2D start,
                                                   point2D end, int elevation, int height) {
        if (elevation < 0) {
            return {};
        }
        return std::make_shared<solidWall>(solidWall(start,
                                                     end, elevation, height, 120, material::brick_wall));
    }
};


enum class wallType {
    basic,
    main,
    partition
};

class wallFactory {
    static std::vector<std::weak_ptr<wall> > walls;

protected:
    static std::shared_ptr<solidWall> creat_main(point2D start, point2D end, int elevation, int height) {
        if (elevation < 0) {
            return {};
        }
        auto this_wall=std::make_unique<solidWall>(solidWall::creat_main(start, end, elevation, height));
        // auto this_wall = std::make_unique<solidWall>(solidWall(start, end, elevation, height, 375, material::gas_wall));
        // 在创建墙体时，检查是否有其他墙体与当前墙体相交 , 如果存在相交情况则不会构建墙体
        // auto temp_ptr = std::make_shared<solidWall>(start, end, elevation, height, 375, material::gas_wall);
        for (const auto wall: walls) {
            // 使用weak_ptr的lock方法将weak_ptr转换为shared_ptr
            if (auto p = wall.lock()) {
                if (this_wall->intersects(*p)) {
                    this_wall.reset();
                    // delete this_wall;
                    return {};
                }
            }
        }
        auto temp_ptr = std::shared_ptr<solidWall>(std::move(this_wall));
        walls.push_back(temp_ptr);
        return temp_ptr;
    }

    static std::shared_ptr<solidWall> creat_partition(point2D start, point2D end, int elevation, int height) {
        auto temp_ptr = solidWall::creat_partition(start, end, elevation, height);
        std::shared_ptr<solidWall> temp_shared_ptr = std::move(temp_ptr);
        for (const auto &wall: walls) {
            // 使用weak_ptr的lock方法将weak_ptr转换为shared_ptr
            if (auto p = wall.lock()) {
                if (temp_shared_ptr->intersects(*p)) {
                    temp_shared_ptr.reset();
                    return {};
                }
            }
        }
        walls.push_back(temp_shared_ptr);
        return temp_shared_ptr;
    }

public:
    static std::shared_ptr<wall> create_wall(wallType type, point2D start, point2D end, int elevation,
                                             int height) {
        switch (type) {
            case wallType::main:
                return creat_main(start, end, elevation, height);
            case wallType::partition:
                return creat_partition(start, end, elevation, height);
            case wallType::basic:
                return std::make_shared<wall>(start, end, elevation, height);
            default:
                break;
        }
        return {};
    }
};

std::vector<std::weak_ptr<wall> > wallFactory::walls;

void testFactoryMethod() {
    point2D start{0, 0}, end{0, 3000};
    const auto main_wall = solidWall::creat_main({0, 0}, {0, 3000}, 2700, 3000);
    std::cout << main_wall;
}

void test2() {
    const auto base_wall = wallFactory::create_wall(wallType::basic, {0, 0}, {1, 1}, 0, 1);
    const auto main_wall = wallFactory::create_wall(wallType::main, {1, 1}, {1, 1}, 0, 1);
    const auto partition_wall = wallFactory::create_wall(wallType::partition, {0, 0}, {1, 1}, 0, 1);
    // std::cout << *base_wall;
    // std::cout << *main_wall;
    if (main_wall) {
        auto ptr = std::dynamic_pointer_cast<solidWall>(main_wall);
        if (ptr == nullptr) {
            std::cout << "error" << "\n";
        }
        std::cout << *std::dynamic_pointer_cast<solidWall>(main_wall);
    }
}

int main() {
    testFactoryMethod();
    test2();
    return 0;
}
