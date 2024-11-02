//
// Created by renduiyang on 24-11-2.
//
#include <zmq.hpp>
#include <string>
#include <iostream>
#include <thread>

int main() {
    // 创建 ZeroMQ 上下文
    zmq::context_t context;

    // 创建 PUSH 套接字
    zmq::socket_t push_socket(context, ZMQ_PUSH);

    // 绑定套接字到指定的地址和端口
    push_socket.bind("tcp://*:5555");

    std::cout << "PUSH socket started, sending tasks..." << std::endl;

    // 发送任务
    for (int i = 0; i < 10; ++i) {
        std::string task = "Task " + std::to_string(i);
        zmq::message_t msg(task.begin(), task.end());

        // 发送任务
        push_socket.send(msg, zmq::send_flags::none);

        std::cout << "Sent: " << task << std::endl;

        // 等待一段时间
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    printf("Press any key to exit...\n");
    system("pause");
    return 0;
}
