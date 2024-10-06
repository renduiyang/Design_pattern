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

    /**
     * @brief 重点在这里  使用虚析构函数可以明确表示多态性
     * @note  如果使用dynamic_pointer_cast 会检查多态性 除非有其他虚函数 否则最直接的就是利用虚析构函数显示表明多态性
     */
    virtual ~wall() {
        std::cout << "destruct wall\n";
    }

    bool intersects(const wall &other) const {
        return false;
    }

    /**
     * @brief << 重载运算符  返回类型一定是std::ostream&
     * @param os
     * @param w
     * @return
     */
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
    /**
     * @brief 这里构造函数是proteced类型  make_shared和make_unique是无法使用protected类型的构造函数
     * @param start
     * @param end
     * @param elevation
     * @param height
     * @param width
     * @param material_
     */
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
     * @note make_unique无法使用protected类型的构造函数,因此需要使用普通构造函数构造成功后转换为unique_ptr
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


/**
 * @brief 工厂类  将构造全部封装起来 对外只保留一个接口
 */
class wallFactory {
    static std::vector<std::weak_ptr<wall> > walls;

protected:
    static std::shared_ptr<solidWall> creat_main(point2D start, point2D end, int elevation, int height) {
        if (elevation < 0) {
            return {};
        }
        auto this_wall = std::make_unique<solidWall>(solidWall::creat_main(start, end, elevation, height));
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
    /**
     * @brief
     * @param type
     * @param start
     * @param end
     * @param elevation
     * @param height
     * @return std::shared_ptr<wall> 类型,因为这个函数返回值有的是solidWall有的是wall类型
     * @note 因为直接返回父类shared_ptr指针 所以此处明确声明返回值类型为std::shared_ptr<wall>  !!不能使用auto
     */
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
    if (main_wall) {
        // 因为 solidWall 是 wall 的子类 所以可以转换为shared_ptr<solidWall>
        // 此处转换之后 会返回一个shared_ptr<solidWall>
        auto ptr = std::dynamic_pointer_cast<solidWall>(main_wall);
        if (ptr == nullptr) {
            std::cout << "error" << "\n";
        }
        // 如果要使用这个智能指针需要使用*  因为不带* 这个变量只是一个地址
        std::cout << *std::dynamic_pointer_cast<solidWall>(main_wall);
    }
}

int main() {
    testFactoryMethod();
    test2();
    return 0;
}
