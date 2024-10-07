//
// Created by renduiyang on 24-10-7.
//
#include <iostream>
#include <memory>

class address {
public:
    std::string street, city;
    int suite;

    virtual ~address() {
    }

    address(const std::string &street, const std::string &city, int suite) : street(street), city(city), suite(suite) {
    }

    virtual void myprint() {
        std::cout << "street: " << street << " city: " << city << " suite: " << suite << std::endl;
    }

    virtual address *clone() {
        return new address(street, city, suite);
    }
};

class contact {
private:
    std::string name;
    address *addr;

public:
    contact(const std::string &name, address *addr) : name(name), addr(addr) {
    }

    /**
     * @brief
     * @param other
     * @note 为当前对象动态分配一个新的 address 对象，并通过调用 address 类的拷贝构造函数来复制 other 对象中的 address 数据
     * @note 这里会调用address的拷贝构造函数来来复制otrher的address数据--->(这里必须在address里面声明拷贝构造函数)
     */
    contact(const contact &other): name(other.name), addr(new address(*other.addr)) {
    }

    ~contact() { delete addr; }

    /**
     * @brief
     * @param other
     * @return contact类型 *this：是对 this 指针进行解引用，表示当前对象本身。
     * @note other 是一个引用参数，它实际上是 const contact & 类型。取 other 的地址 &other 就得到了指向 other 对象的指针。
     * @note  引用传入参数  对其进行取址可以得到指向该引用地址对象的指针
     */
    contact &operator=(const contact &other) {
        if (&other == this) {
            return *this;
        }
        name = other.name;
        addr = other.addr;
        return *this;
    }
};

class extenAddress : public address {
public:
    std::string country;

    extenAddress(const std::string &street, const std::string &city, int suite,
                 const std::string &country) : address(street, city, suite), country(country) {
    }

    void myprint() override {
        std::cout << "this is extenAddress" << std::endl;
        address::myprint();
        std::cout << "country: " << country << std::endl;
    }

    extenAddress *clone() override {
        return new extenAddress(street, city, suite, country);
    }
};

class exaddress : public address {
public:
    std::string own_name_;

    exaddress(const std::string &street, const std::string &city, int suite,
              const std::string &own_name) : address(street, city, suite), own_name_(own_name) {
    }

    void myprint() override {
        std::cout << "this is exAddress" << std::endl;
        address::myprint();
        std::cout << "own_name: " << own_name_ << std::endl;
    }

    exaddress *clone() override {
        return new exaddress(street, city, suite, own_name_);
    }
};

int main() {
    extenAddress addr("123 street", "xian", 123, "china");
    address &a = addr;
    // a.myprint();
    const auto ptr = a.clone(); // 调用clone函数这个指针类型是address* 但是指向了extendAddress
    const auto ea_ptr = dynamic_cast<extenAddress *>(ptr);
    ea_ptr->myprint();
    return 0;
}
