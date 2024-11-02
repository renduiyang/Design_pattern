//
// Created by renduiyang on 24-11-2.
//
#include <zmq.hpp>
#include <string>
#include <iostream>
#include <thread>

void worker(int id) {
    // 创建 ZeroMQ 上下文
    zmq::context_t context;

    // 创建 PULL 套接字
    zmq::socket_t pull_socket(context, ZMQ_PULL);

    // 连接到 PUSH 套接字的地址和端口
    pull_socket.connect("tcp://localhost:5555");

    std::cout << "Worker " << id << " started, waiting for tasks..." << std::endl;

    // 接收任务
    while (true) {
        zmq::message_t msg;
        if (pull_socket.recv(msg, zmq::recv_flags::none)) {
            std::string task(static_cast<char *>(msg.data()), msg.size());
            std::cout << "Worker " << id << " received: " << task << std::endl;

            // 模拟处理任务
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}

int main() {
    // 启动多个工作线程
    std::thread workers[3];
    for (int i = 0; i < 3; ++i) {
        workers[i] = std::thread(worker, i + 1);
    }

    // 等待所有工作线程完成
    for (auto &t: workers) {
        t.join();
    }

    printf("Press any key to exit...\n");
    system("pause");
    return 0;
}
