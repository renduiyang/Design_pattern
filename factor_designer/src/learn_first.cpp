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

    bool intersects(const wall &other) const {
        return false;
    }

    friend std::ostream &operator <<(std::ostream os, const wall &w) {
        return os << "wall from " << "(" << w.start.x << w.start.y << ")" << " to " << "(" << w.start.x << w.start.y <<
               ")" << " with elevation " << w.elevation << " and height "
               << w.height;
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
    int width;
    material material_;

protected:
    solidWall(point2D start, point2D end, int elevation, int height, int width, material material_) : wall(start, end,
            elevation,
            height),
        width(width),
        material_(
            material_) {
    }

public:
    static solidWall creat_main(point2D start, point2D end, int elevation, int height) {
        return solidWall(start, end, elevation, height, 375, material::gas_wall);
    }

    static auto creat_partition(point2D start, point2D end, int elevation, int height) -> std::unique_ptr<solidWall> {
        return std::make_unique<solidWall>(start, end, elevation, height, 250, material::drywall);;
    }

    friend std::ostream &operator<<(std::ostream os, const solidWall &w) {
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
        return os << "wall from " << "(" << w.start.x << w.start.y << ")" << " to " << "(" << w.start.x << w.start.y <<
               ")" << " with elevation " << w.elevation << " and height "
               << w.height << " and width"
               << w.width << " and material " << material_name;
    }
};

class wallFactory {
    static std::pmr::vector<std::weak_ptr<wall> > walls;

public:
    static auto creat_main(point2D start, point2D end, int elevation, int height) -> std::shared_ptr<wall> {
        if (elevation < 0) {
            return nullptr;
        }
        // 在创建墙体时，检查是否有其他墙体与当前墙体相交 , 如果存在相交情况则不会构建墙体
        auto temp_ptr = std::make_shared<solidWall>(start, end, elevation, height, 375, material::gas_wall);
        for (const auto &wall: walls) {
            // 使用weak_ptr的lock方法将weak_ptr转换为shared_ptr
            if (auto p = wall.lock()) {
                if (temp_ptr->intersects(*p)) {
                    temp_ptr.reset();
                    return nullptr;
                }
            }
        }
        walls.push_back(temp_ptr);
        return temp_ptr;
    }

    static auto creat_partition(point2D start, point2D end, int elevation, int height) -> std::shared_ptr<wall> {
        auto temp_ptr = solidWall::creat_partition(start, end, elevation, height);
        std::shared_ptr<solidWall> temp_shared_ptr = std::move(temp_ptr);
        for (const auto &wall: walls) {
            // 使用weak_ptr的lock方法将weak_ptr转换为shared_ptr
            if (auto p = wall.lock()) {
                if (temp_shared_ptr->intersects(*p)) {
                    temp_shared_ptr.reset();
                    return nullptr;
                }
            }
        }
        walls.push_back(temp_shared_ptr);
        return temp_shared_ptr;
    }
};
