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

template<typename T>
struct specification {
    virtual bool is_satisfied(T *item) =0;
};

template<typename T>
struct Filter {
    virtual std::vector<T *> filter(std::vector<T *> items, specification<T> &spec) const =0;
};

struct BatterFilter : Filter<product> {
    std::vector<product *> filter(std::vector<product *> items, specification<product> &spec) const override {
        std::vector<product *> result;
        for (auto &value: items) {
            if (spec.is_satisfied(value)) {
                result.push_back(value);
            }
        }
        return result;
    }
};

struct ColorSpectification : specification<product> {
    Color _color;

    explicit ColorSpectification(const Color color): _color{color} {
    }

    bool is_satisfied(product *item) override {
        return item->color == _color;
    }
};

struct SizeSpectification : specification<product> {
    Size _size;

    explicit SizeSpectification(const Size size): _size(size) {
    }

    bool is_satisfied(product *item) override {
        return item->size == _size;
    }
};

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

void test1() {
    product apple{"apple", Color::red, Size::small};
    product tree{"tree", Color::green, Size::large};
    product house{"house", Color::green, Size::large};
    std::vector<product *> all{&apple, &tree, &house};

    BatterFilter bf;
    ColorSpectification green(Color::green);
    SizeSpectification large(Size::large);
    AndSpectification<product> and_spec(large, green);
    auto green_things = bf.filter(all, and_spec);
    for (const auto &value: green_things) {
        std::cout << value->name << std::endl;
    }
}

int main() {
    test1();
    return 0;
}
