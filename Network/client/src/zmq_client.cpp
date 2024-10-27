#include <zmq.hpp>
#include <iostream>
#include <string>
#include "person.h"
#include <vector>

int main() {
    try {
        // 创建 ZeroMQ 上下文
        zmq::context_t context(1);

        // 创建订阅者套接字
        zmq::socket_t subscriber(context, ZMQ_SUB);
        subscriber.connect("tcp://localhost:5556");
        subscriber.set(zmq::sockopt::subscribe, "");
        printf("set success \n");
        while (true) {
            // 接收数据
            zmq::message_t message;
            subscriber.recv(message, zmq::recv_flags::none);
            printf("Received message of %llu bytes\n", message.size());
            // 反序列化对象
            std::vector<uint8_t> data(static_cast<const uint8_t *>(message.data()),
                                      static_cast<const uint8_t *>(message.data()) + message.size());
            Person p2 = Person::deserialize(data);

            std::cout << "Received: " << p2.name << ", " << p2.age << std::endl;

            // 仅接收一次消息后退出
            break;
        }
    } catch (const zmq::error_t &e) {
        std::cerr << "ZMQ Error: " << e.what() << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    system("pause");
    return 0;
}
