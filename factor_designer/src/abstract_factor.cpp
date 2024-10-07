//
// Created by renduiyang on 24-10-6.
//
#include <memory>
#include <iostream>
#include <map>

class hotDrink {
public:
    virtual ~hotDrink() = default;

    virtual void prepare(int volume) = 0;
};

class tea : public hotDrink {
protected:
    void prepare(int volume) override {
        std::cout << "take tea bag, boil water, pour " << volume << "ml, add some lemon" << std::endl;
    }
};

class coffee : public hotDrink {
protected:
    void prepare(int volume) override {
        std::cout << "grind some beans, boil water, pour " << volume << "ml, add cream and sugar" << std::endl;
    }
};

// ----此处根据饮品名来实现,每次都要修改这个函数
std::unique_ptr<hotDrink> make_drink(const std::string &type) {
    std::unique_ptr<hotDrink> drink;
    if (type == "tea") {
        drink = std::make_unique<tea>();
        drink->prepare(200);
    } else {
        drink = std::make_unique<coffee>();
        drink->prepare(50);
    }
    return drink;
}

// ---------使用抽象工厂来实现
// 此处hotdrinkFactor可以抽象出所有饮品的工厂 是一个虚基类
class hotDrinkFactor {
public:
    virtual std::unique_ptr<hotDrink> make() const = 0;
    virtual ~hotDrinkFactor() = default;
};

class tea_ProductionLine : public hotDrinkFactor {
public:
    std::unique_ptr<hotDrink> make() const override {
        return std::make_unique<tea>();
    }
};

class coffee_ProductionLine : public hotDrinkFactor {
public:
    std::unique_ptr<hotDrink> make() const override {
        return std::make_unique<coffee>();
    }
};

class drinkFactor {
    std::map<std::string, std::unique_ptr<hotDrinkFactor> > hot_factories;

public:
    drinkFactor() {
        hot_factories["tea"] = std::make_unique<tea_ProductionLine>();
        hot_factories["coffee"] = std::make_unique<coffee_ProductionLine>();
    }

    std::unique_ptr<hotDrink> make_drink(const std::string &name) {
        auto drink = hot_factories[name]->make();
        drink->prepare(200);
        return drink;
    }
};

int main() {
    auto tea_drink = make_drink("tea");
    drinkFactor df;
    auto coffee_drink = df.make_drink("coffee");
    return 0;
}
