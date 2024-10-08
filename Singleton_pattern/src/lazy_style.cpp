//
// Created by renduiyang on 24-10-8.
//
#include <iostream>
#include <vector>
#include <thread>

/**
 * @brief 静态局部变量的懒汉单例 这种线程安全的
 */
class lazyStyle {
public:
    static lazyStyle &getInstance() {
        static lazyStyle instance;
        return instance;
    }

    void myprint() {
        printf_s("LazyStyle::myprint address is %p \n", this);
        printf_s("LazyStyle::myprint size is %llu \n", sizeof(lazyStyle));
    }

    void make_data() {
        for (int i = 0; i < 100; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1)); // 休眠1ms
            ++box_size; // 写入操作：counter自加
        }
        // box_size++;
    }

    void chang_data(const int i) {
        if (i < box_size) {
            box[i] = 100;
        } else {
            box[0] = 100;
        }
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
    demo2();
    return 0;
}
