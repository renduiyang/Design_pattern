//
// Created by renduiyang on 24-10-27.
//
#include <zmq.hpp>
#include <iostream>
#include <string>
#include "person.h"
#include <vector>
#include <thread>
#include <chrono>

int main() {
    try {
        // 创建 ZeroMQ 上下文
        zmq::context_t context(1);

        // 创建发布者套接字
        zmq::socket_t publisher(context, ZMQ_PUB);
        publisher.bind("tcp://*:5556");

        // 添加延迟，确保订阅者有时间启动
        std::this_thread::sleep_for(std::chrono::seconds(5));

        // 创建一个 Person 对象
        Person p1("Alice", 30);

        // 序列化对象
        std::vector<uint8_t> serializedData = p1.serialize();
        printf("begin send data");
        // 发送数据
        zmq::message_t message(serializedData.data(), serializedData.size());
        publisher.send(message, zmq::send_flags::none);

        std::cout << "Sent: " << p1.name << ", " << p1.age << std::endl;
    } catch (const zmq::error_t &e) {
        std::cerr << "ZMQ Error: " << e.what() << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    system("pause");
    return 0;
}
