#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <queue>
#include <conio.h> // 用于 _getch()

#pragma comment(lib, "ws2_32.lib")

// 初始化Winsock库
void InitializeWinsock() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        exit(1);
    }
}

// 连接到服务器
SOCKET ConnectToServer(const char* ip, const char* port) {
    struct addrinfo hints, *res, *p;
    int result;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // 解析服务器地址
    result = getaddrinfo(ip, port, &hints, &res);
    if (result != 0) {
        std::cerr << "getaddrinfo failed: " << gai_strerror(result) << std::endl;
        return INVALID_SOCKET;
    }

    SOCKET sockfd = INVALID_SOCKET;
    for (p = res; p != NULL; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == INVALID_SOCKET) {
            continue;
        }

        // 尝试连接
        if (connect(sockfd, p->ai_addr, (int)p->ai_addrlen) == SOCKET_ERROR) {
            closesocket(sockfd);
            sockfd = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(res);

    if (sockfd == INVALID_SOCKET) {
        std::cerr << "Failed to connect to server" << std::endl;
    }

    return sockfd;
}

// 发送消息
bool SendMessage(SOCKET sockfd, const std::string& message) {
    int n = send(sockfd, message.c_str(), message.length(), 0);
    if (n == SOCKET_ERROR) {
        std::cerr << "send failed: " << WSAGetLastError() << std::endl;
        return false;
    }
    return true;
}

// 接收消息
bool ReceiveMessage(SOCKET sockfd, std::string& message) {
    char buffer[1024];
    int n = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (n > 0) {
        buffer[n] = '\0';
        message = buffer;
        return true;
    } else if (n == 0) {
        std::cerr << "Connection closed by server." << std::endl;
    } else {
        std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
    }
    return false;
}

// 接收确认
bool ReceiveAck(SOCKET sockfd) {
    std::string response;
    if (ReceiveMessage(sockfd, response)) {
        if (response == "ACK") {
            return true; // 收到确认
        } else {
            std::cerr << "Unexpected response: " << response << std::endl;
        }
    }
    return false; // 没有收到确认或错误
}

// 心跳包发送线程
void HeartbeatThread(SOCKET sockfd, std::atomic<bool>& running, std::condition_variable& cv, std::mutex& mtx) {
    std::unique_lock<std::mutex> lock(mtx);
    while (running) {
        cv.wait_for(lock, std::chrono::seconds(60)); // 每隔60秒发送一次心跳包

        if (!running) {
            break;
        }

        if (!SendMessage(sockfd, "HEARTBEAT:")) {
            std::cerr << "Failed to send heartbeat, connection may be lost." << std::endl;
            running = false;
            break;
        }

        if (!ReceiveAck(sockfd)) {
            std::cerr << "No ACK received, connection may be lost." << std::endl;
            running = false;
            break;
        } else {
            std::cout << "Heartbeat sent and ACK received." << std::endl;
        }
    }
}

// 消息接收线程
void MessageReceiverThread(SOCKET sockfd, std::atomic<bool>& running, std::queue<std::string>& messageQueue, std::mutex& queueMtx) {
    while (running) {
        std::string message;
        if (ReceiveMessage(sockfd, message)) {
            std::lock_guard<std::mutex> lock(queueMtx);
            messageQueue.push(message);
        } else {
            running = false;
            break;
        }
    }
}

// 键盘监听线程
void KeyboardListenerThread(std::atomic<bool>& running) {
    while (running) {
        if (_kbhit()) { // 检测是否有按键输入
            char key = _getch();
            if (key == 27) { // 27 是 Esc 键的 ASCII 码
                std::cout << "Esc key pressed. Closing client..." << std::endl;
                running = false;
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 避免 CPU 占用过高
    }
}

// 主循环
void ClientLoop(SOCKET sockfd, std::atomic<bool>& running, std::queue<std::string>& messageQueue, std::mutex& queueMtx) {
    while (running) {
        // 发送其他数据
        if (!SendMessage(sockfd, "DATA:Hello, Server!")) {
            std::cerr << "Failed to send data, connection may be lost." << std::endl;
            running = false;
            break;
        }

        // 处理消息队列
        {
            std::lock_guard<std::mutex> lock(queueMtx);
            while (!messageQueue.empty()) {
                std::string message = messageQueue.front();
                messageQueue.pop();
                std::cout << "Received from server: " << message << std::endl;
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(10)); // 每隔10秒发送一次数据
    }
}

int main() {
    InitializeWinsock();

    SOCKET sockfd = ConnectToServer("127.0.0.1", "8080");
    if (sockfd == INVALID_SOCKET) {
        std::cerr << "Failed to connect to the server. Exiting..." << std::endl;
        WSACleanup();
        return 1;
    }

    std::atomic<bool> running(true);
    std::condition_variable cv;
    std::mutex mtx;
    std::queue<std::string> messageQueue;
    std::mutex queueMtx;

    // 启动心跳包发送线程
    std::thread heartbeatThread(HeartbeatThread, sockfd, std::ref(running), std::ref(cv), std::ref(mtx));

    // 启动消息接收线程
    std::thread receiverThread(MessageReceiverThread, sockfd, std::ref(running), std::ref(messageQueue), std::ref(queueMtx));

    // 启动键盘监听线程
    std::thread keyboardThread(KeyboardListenerThread, std::ref(running));

    try {
        // 启动主循环
        ClientLoop(sockfd, running, messageQueue, queueMtx);
    } catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception caught." << std::endl;
    }

    // 停止心跳包发送线程
    running = false;
    cv.notify_all(); // 通知条件变量，使心跳线程退出
    heartbeatThread.join();

    // 停止消息接收线程
    receiverThread.join();

    // 停止键盘监听线程
    keyboardThread.join();

    closesocket(sockfd);
    WSACleanup();
    return 0;
}