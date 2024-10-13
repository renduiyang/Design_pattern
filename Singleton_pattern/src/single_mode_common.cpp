//
// Created by renduiyang on 24-10-12.
//
#include <iostream>
#include <mutex>
#include <thread>

std::mutex style_mutex;

namespace lazy_style {
    class laze_test {
        // 虽然类中默认是private的可以不用显示声明private 但是声明出来代码阅读起来会更清晰
    private:
        // 如果没有什么特殊 可以使用default 构造函数
        laze_test() = default;

        ~laze_test() = default;

        laze_test(const laze_test &) = delete;

        laze_test &operator=(const laze_test &) = delete;

        static laze_test *instance;

    public:
        static laze_test *get_instance() {
            if (instance == nullptr) {
                // lock_guard符合RAII思维
                std::lock_guard<std::mutex> guard(style_mutex);
                // style_mutex.lock();
                if (instance == nullptr) {
                    instance = new laze_test();
                }
                // style_mutex.unlock();
            }
            return instance;
        }

        void exampleFunction() {
            printf("address is %p \n", this);
            std::cout << "function name is   : " << __FUNCTION__ << std::endl;
            printf("__PRETTY_FUNCTION__ = %s\n", __PRETTY_FUNCTION__);

            // Sample Output
            // -------------------------------------------------
            // 函数名  : exampleFunction
            // 函数修饰: ? exampleFunction@@YAXXZ
            // 函数签名 : void __cdecl exampleFunction(void)
        }
    };
}

// 本身就是类名  类名::静态类型
// 因为加了一层命名空间所以为 命名空间::类名  命名空间::类名::静态类型
lazy_style::laze_test *lazy_style::laze_test::instance = nullptr;

namespace hungry_style {
    class hungry_test {
    private:
        hungry_test() = default;

        ~hungry_test() = default;

        hungry_test(const hungry_test &) = delete;

        hungry_test &operator=(const hungry_test &) = delete;

        static hungry_test hungry_instance;

    public:
        static hungry_test &get_instance() {
            return hungry_instance;
        }

        void exampleFunction() {
            printf("address is %p \n", this);
            std::cout << "function name is   : " << __FUNCTION__ << std::endl;
            printf("__PRETTY_FUNCTION__ = %s\n", __PRETTY_FUNCTION__);

            // Sample Output
            // -------------------------------------------------
            // 函数名  : exampleFunction
            // 函数修饰: ? exampleFunction@@YAXXZ
            // 函数签名 : void __cdecl exampleFunction(void)
        }
    };
}

hungry_style::hungry_test hungry_style::hungry_test::hungry_instance;

void demo1() {
    using namespace lazy_style;
    using namespace hungry_style;
    laze_test *lazy_instance = laze_test::get_instance();
    lazy_instance->exampleFunction();
    laze_test *laze_instance2 = laze_test::get_instance();
    std::cout << "lazy_instance == laze_instance2 is " << (lazy_instance == laze_instance2) << std::endl;
    laze_instance2->exampleFunction();
    printf("-------- \n");
    hungry_test &hungry_instance = hungry_test::get_instance();
    hungry_instance.exampleFunction();
    hungry_test &hungry_instance2 = hungry_test::get_instance();
    std::cout << "hungry_instance == hungry_instance2 is " << (&hungry_instance == &hungry_instance2) << std::endl;
    hungry_instance2.exampleFunction();
}

int main() {
    demo1();
    return 0;
}
