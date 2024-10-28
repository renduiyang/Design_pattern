#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

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
SOCKET ConnectToServer(const char *ip, const char *port) {
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
        if (connect(sockfd, p->ai_addr, (int) p->ai_addrlen) == SOCKET_ERROR) {
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

// 发送心跳包
bool SendHeartbeat(SOCKET sockfd) {
    const char *heartbeat = "HEARTBEAT";
    int n = send(sockfd, heartbeat, strlen(heartbeat), 0);
    if (n == SOCKET_ERROR) {
        std::cerr << "send failed: " << WSAGetLastError() << std::endl;
        return false;
    }
    return true;
}

// 接收确认
bool ReceiveAck(SOCKET sockfd) {
    char buffer[1024];
    int n = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (n > 0) {
        buffer[n] = '\0';
        if (strncmp(buffer, "ACK", 3) == 0) {
            return true; // 收到确认
        } else {
            std::cerr << "Unexpected response: " << buffer << std::endl;
        }
    } else if (n == 0) {
        std::cerr << "Connection closed by server." << std::endl;
    } else {
        std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
    }
    return false; // 没有收到确认或错误
}

// 主循环
void ClientLoop(SOCKET sockfd) {
    while (true) {
        if (!SendHeartbeat(sockfd)) {
            std::cout << "Connection lost, attempting to reconnect..." << std::endl;
            closesocket(sockfd);
            sockfd = ConnectToServer("127.0.0.1", "8080");
            if (sockfd == INVALID_SOCKET) {
                std::this_thread::sleep_for(std::chrono::seconds(5)); // 等待一段时间再重试
                continue;
            }
        }

        if (!ReceiveAck(sockfd)) {
            std::cout << "No ACK received, connection may be lost." << std::endl;
            closesocket(sockfd);
            sockfd = ConnectToServer("127.0.0.1", "8080");
            if (sockfd == INVALID_SOCKET) {
                std::this_thread::sleep_for(std::chrono::seconds(5)); // 等待一段时间再重试
            }
        } else {
            std::cout << "Connection is alive." << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::seconds(5)); // 每隔5秒发送一次心跳包
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

    try {
        // 启动主循环
        ClientLoop(sockfd);
    } catch (const std::exception &e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception caught." << std::endl;
    }

    closesocket(sockfd);
    WSACleanup();
    return 0;
}
