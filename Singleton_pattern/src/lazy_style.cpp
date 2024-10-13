//
// Created by renduiyang on 24-10-8.
//
#include <iostream>
#include <vector>
#include <thread>

/**
 * @brief 静态局部变量的懒汉单例 这种线程安全的(因为getInstance函数是被声明为static)
 * @note 这是一种写法
 * @note movzbl	guard variable for lazyStyle::getInstance()::instance(%rip), %eax 扩展守护变量
 * @note leaq	guard variable for lazyStyle::getInstance()::instance(%rip), %rax 加载静态变量时 使用守护变量保护这个初始哈过程
 * @note 在多线程环境中，静态局部变量的初始化需要保证线程安全。编译器通常会生成一些额外的代码来保护这个初始化过程。其中，“守护变量”（guard variable）是一种常见的方法。
 */
class lazyStyle {
public:
    // 这个getInstance函数被声明为static，一定是线程安全的
    static lazyStyle &getInstance() {
        static lazyStyle instance;
        return instance;
    }


    void make_data() {
        for (int i = 0; i < 100; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1)); // 休眠1ms
            ++box_size; // 写入操作：counter自加
        }
    }

    void chang_data(const int i) {
        if (i < box_size) {
            box[i] = 100;
        } else {
            box[0] = 100;
        }
    }

    void myprint() {
        printf_s("LazyStyle::myprint address is %p \n", this);
        printf_s("LazyStyle::myprint size is %llu \n", sizeof(lazyStyle));
    }

    void show_data() {
        for (auto &i: box) {
            printf_s("%d ", i);
        }
        printf_s("\n");
    }

    int show_endData() const {
        return box_size;
    }

private:
    int box_size = 0;
    std::vector<int> box = {1, 2, 3, 4, 5};

    lazyStyle() {
    }

    lazyStyle(lazyStyle const &) = delete;

    void operator=(lazyStyle const &) = delete;
};

lazyStyle *ptr = &lazyStyle::getInstance();


void change(const int i) {
    lazyStyle::getInstance().chang_data(i);
}

void demo1() {
    std::thread thread1([] {
        printf("thread1 \n");
        lazyStyle::getInstance().make_data();
    });

    std::thread thread2([] {
        printf("thread2 \n");
        lazyStyle::getInstance().make_data();
    });

    thread1.join();
    thread2.join();

    printf_s("end data is %d \n", lazyStyle::getInstance().show_endData());
}

int counter = 0;

void func() {
    for (int i = 0; i < 100; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1)); // 休眠1ms
        ++counter; // 写入操作：counter自加
    }
}

/**
 * @brief 线程不安全示例
 */
void demo2() {
    std::thread t1 = std::thread(func);
    std::thread t2 = std::thread(func);

    t1.join();
    t2.join();

    printf("counter is %d \n", counter);
}


int main() {
    printf("main thread \n");
    for (int i = 0; i < 20; ++i) {
        printf("-------- \n");
        demo1();
    }
    return 0;
}
