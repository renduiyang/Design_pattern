//
// Created by renduiyang on 24-9-22.
//
#include <iostream>

namespace static_test {
    template<typename TImpl>
    class Notifier {
        friend TImpl; // 此处声明TImpl为友元类,允许访问Notifier的私有成员
    public:
        void AlertSMS(std::string_view msg) {
            impl().SendAltertSMS(msg);
        }

        void AlertEmail(std::string_view msg) {
            impl().SendAltertEmail(msg);
        }

    private:
        TImpl impl() {
            return static_cast<TImpl &>(*this);
        }
    };

    template<typename TImpl>
    void Alert(Notifier<TImpl> &notifier, std::string_view msg) {
        notifier.AlertSMS(msg);
        notifier.AlertEmail(msg);
    }

    class SMSNotifier : public Notifier<SMSNotifier> {
    public:
        void SendAltertSMS(std::string_view msg) {
            std::cout << "Send SMS: " << msg << std::endl;
        }

        void SendAltertEmail(std::string_view msg) {
            std::cout << "Send Email: " << msg << std::endl;
        }
    };

    void test1() {
        SMSNotifier notifier;
        Alert(notifier, "hello world");
    }
}


namespace concept_test {
    /**
     * @brief c++20 concept代码概念----模板参数ITmpl必须实现AlertSMS和AlertEmail两个函数
     * requires 是一个关键字，主要用于概念约束（concept constraints）中，帮助编译器进行更严格的类型检查
     */
    template<typename ITmpl>
    concept IsANotifier = requires(ITmpl notifier, std::string_view msg)
    {
        notifier.AlertSMS(msg);
        notifier.AlertEmail(msg) ;
    };

    /**
     *
     * @tparam TImpl 使用IsANotifier来约束模板参数
     * @param impl
     * @param msg
     */
    template<IsANotifier TImpl>
    void Alert(TImpl &impl, std::string_view msg) {
        impl.AlertSMS(msg);
        impl.AlertEmail(msg);
    }

    class SMSNotifier {
    public:
        void AlertSMS(std::string_view msg) {
            std::cout << "Send SMS: " << msg << std::endl;
        }

        void AlertEmail(std::string_view msg) {
            std::cout << "Send Email: " << msg << std::endl;
        }
    };

    /**
     *  concept只是限制参数有没有具体方法  --t只是判断 参数t是否具有加减的方法
     *
     **/
    template<typename T>
    concept Decrementable = requires(T t)
    {
        --t;
    };

    /**
     * @brief 
     * @tparam T 
     * @param t 
     * @param t2 
     */
    template<Decrementable T>
    void f(T t, T t2) {
        std::cout << "Decrementable" << std::endl;
        ++t;
        std::cout << t << std::endl;
    }


    void test2() {
        SMSNotifier notifier;
        Alert(notifier, "hello world");
        f(42, 0);
    }
}

namespace attribute_test {
    class person {
    private:
        int age;

    public:
        void set_age(int age) {
            this->age = age;
        }

        int get_age() const {
            return age;
        }
    };

    /**
     * @brief 这个是属性访问器的写法
     * @note 借助了函数重载的方法  实现属性访问器
     */
    class person2 {
    private:
        int _age;

    public:
        int age() const {
            return this->_age;
        }

        void age(int _age) {
            this->_age = _age;
        }
    };

    void test3() {
        person p1;
        p1.set_age(10);
        std::cout << p1.get_age() << std::endl;
        std::cout << "-----------" << std::endl;
        person2 p2;
        p2.age(20);
        std::cout << p2.age() << std::endl;
    }
}

int main() {
    // static_test::test1();
    // std::cout << "----------" << std::endl;
    // concept_test::test2();
    // std::cout << "----------" << std::endl;
    attribute_test::test3();
    return 0;
}
