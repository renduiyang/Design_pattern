#include<iostream>
#include<typeinfo>


/**
 * @brief use common_template and CRTP; use CRTP achieve static polymorphism
 * @note CRTP use static_cast<T*>() change base ptr to child ptr achieve polymorphism
 * @note common polymorphism use virtual function achieve
 */
namespace common_template {
    class Base {
    protected:
        ~Base() = default;

    public:
        virtual void PrintType() const {
            std::cout << typeid(*this).name() << std::endl;
        }
    };

    class Derived1 : public Base {
    };

    class Derived2 : public Base {
    };

    /**
    * @brief
    * @param base
    * @note CRTP是不需要使用virtual字符 直接就可以实现多态
    * @note 如果是普通继承的话，需要使用virtual
    */
    void PrintType(const Base &base) {
        base.PrintType();
    }

    void PrintType(const Base *b1) {
        b1->PrintType();
    }
}

namespace CRTP_template {
    template<typename T>
    class Base {
    protected:
        ~Base() = default;

    public:
        void PrintType() {
            T &t = static_cast<T &>(*this);
            std::cout << typeid(t).name() << std::endl;
        }
    };

    class Derived1 : public Base<Derived1> {
    };

    class Derived2 : public Base<Derived2> {
    };

    /**
    * @brief
    * @tparam T
    * @param base
    * @note 使用CRTP主要是利用模板来模拟多态  避免了使用虚函数表带来的额外开销
    * @note 传统的继承是通过派生类向基类添加功能，而 CRTP 可以实现通过基类向派生类添加功能，也就是颠倒继承
    * @note 针对本例子,在传统继承时需要在每个子类中实现一个PrintType函数(派生类向基类添加功能)
    * @note 而CRTP则只需要在基类中实现一个函数(基类向派生类添加功能)
    */
    template<typename T>
    void PrintType(T *base) {
        base->PrintType();
    }
}

namespace static_Polymorphism {
    /**
     * @brief CRTP 实现静态多态
     * @tparam T
     */
    template<typename T>
    class Base {
    public:
        // 在结构函数中调用imp方法
        void interface() {
            static_cast<T *>(this)->imp();
        }

        void imp() {
            std::cout << "in Base::imp" << std::endl;
        }
    };

    class Derived1 : public Base<Derived1> {
    public:
        void imp() {
            std::cout << "in Derived1::imp" << std::endl;
        }
    };

    class Derived2 : public Base<Derived2> {
    public:
        void imp() {
            std::cout << "in Derived2::imp" << std::endl;
        }
    };

    class Derived3 : public Base<Derived3> {
    };

    template<typename T>
    void fun(T &base) {
        base.interface();
    }

    void test1() {
        Derived1 d1;
        Derived2 d2;
        fun(d1);
        fun(d2);
    }
}


void test_CRTP() {
    CRTP_template::Derived1 d1;
    CRTP_template::Derived2 d2;

    // PrintType_CRTP(d1);
    // PrintType_CRTP(d2);
    std::cout << "---------" << std::endl;
    CRTP_template::PrintType(&d1);
    CRTP_template::PrintType(&d2);
}

void test1() {
    // 如果基类没有实现虚函数,那么基类指针传入子类对象时每次都会调用基类的虚函数
    common_template::Derived1 d1;
    common_template::Derived2 d2;

    common_template::PrintType(d1);
    common_template::PrintType(d2);
    std::cout << "---------" << std::endl;
}

int main() {
    static_Polymorphism::test1();
    // test_CRTP();
    return 0;
}
