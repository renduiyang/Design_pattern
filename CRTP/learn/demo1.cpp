//
// Created by renduiyang on 24-9-29.
//
#include<iostream>
#include<vector>

enum class Color {
    red,
    green,
    blue
};

enum class Size {
    small,
    medium,
    large
};

struct product {
    std::string name;
    Color color;
    Size size;
};


/**
 * @brief 使用分布式原则  将过滤器判断与过滤器本身分隔
 * @tparam T 模板形参  表示过滤器类型  传入产品参数
 * @note 这里使用使用虚函数是为了后面的多态性 由子类实现这个函数
 */
template<typename T>
struct specification {
    virtual bool is_satisfied(T *item) =0;
};


/**
 * @brief 过滤器  也是一个模板类型
 * @tparam T 传入产品参数 &&为万能引用  涉及到运算符重载则需要使用万能引用转发
 */
template<typename T>
struct Filter {
    virtual std::vector<T *> filter(std::vector<T *> items, specification<T> &&spec) const =0;
};


/**
 * @brief 实例化过滤器  实现模板方法(虚函数)
 */
struct BatterFilter : Filter<product> {
    std::vector<product *> filter(std::vector<product *> items, specification<product> &&spec) const override {
        std::vector<product *> result;
        for (auto &value: items) {
            if (spec.is_satisfied(value)) {
                result.push_back(value);
            }
        }
        return result;
    }
};


/**
 * @brief 颜色过滤器
 */
struct ColorSpectification : specification<product> {
    Color _color;

    explicit ColorSpectification(const Color color): _color{color} {
    }

    bool is_satisfied(product *item) override {
        return item->color == _color;
    }
};


/**
 * @brief 大小过滤器
 */
struct SizeSpectification : specification<product> {
    Size _size;

    explicit SizeSpectification(const Size size): _size(size) {
    }

    bool is_satisfied(product *item) override {
        return item->size == _size;
    }
};

/**
 * @brief 组合条件过滤器
 * @tparam T 产品类型  也是一个模板类
 */
template<typename T>
struct AndSpectification : specification<T> {
    specification<T> &_first;
    specification<T> &_second;

    AndSpectification(specification<T> &first, specification<T> &second): _first{first}, _second{second} {
    }

    // 组合条件
    bool is_satisfied(T *item) override {
        return _first.is_satisfied(item) && _second.is_satisfied(item);
    }
};


template<typename T>
AndSpectification<T> operator&&(specification<T> &first, specification<T> &second) {
    return {first, second};
}


void test1() {
    product apple{"apple", Color::red, Size::small};
    product tree{"tree", Color::green, Size::large};
    product house{"house", Color::green, Size::large};
    std::vector<product *> all{&apple, &tree, &house};

    BatterFilter bf;
    ColorSpectification green(Color::green);
    SizeSpectification large(Size::large);
    // AndSpectification<product> and_spec(large, green);
    auto green_things = bf.filter(all, green && large);
    for (const auto &value: green_things) {
        std::cout << value->name << std::endl;
    }
}

int main() {
    test1();
    return 0;
}
