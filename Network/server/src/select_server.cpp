#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <vector>

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

// 创建并绑定监听套接字
SOCKET CreateAndBindSocket(const char *port) {
    struct addrinfo hints, *res, *p;
    int result;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE; // 用于绑定本地地址

    // 解析服务器地址
    result = getaddrinfo("127.0.0.1", port, &hints, &res);
    if (result != 0) {
        std::cerr << "getaddrinfo failed: " << gai_strerror(result) << std::endl;
        return INVALID_SOCKET;
    }

    SOCKET listenSock = INVALID_SOCKET;
    for (p = res; p != NULL; p = p->ai_next) {
        listenSock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listenSock == INVALID_SOCKET) {
            continue;
        }

        // 绑定套接字
        if (bind(listenSock, p->ai_addr, (int) p->ai_addrlen) == SOCKET_ERROR) {
            closesocket(listenSock);
            listenSock = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(res);

    if (listenSock == INVALID_SOCKET) {
        std::cerr << "Failed to create and bind socket" << std::endl;
    }

    return listenSock;
}

// 处理客户端数据
void HandleClientData(SOCKET clientSock) {
    char buffer[1024];
    int n = recv(clientSock, buffer, sizeof(buffer) - 1, 0);
    if (n > 0) {
        buffer[n] = '\0';
        std::cout << "Received from client: " << buffer << std::endl;

        // 发送回显数据
        if (strncmp(buffer, "HEARTBEAT", 9) == 0) {
            const char *ack = "ACK";
            send(clientSock, ack, strlen(ack), 0);
        } else {
            send(clientSock, buffer, n, 0);
        }
    } else if (n == 0) {
        // 连接关闭
        std::cout << "Connection closed by client." << std::endl;
        closesocket(clientSock);
    } else {
        // 接收错误
        std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
        closesocket(clientSock);
    }
}

int main() {
    InitializeWinsock();

    // 创建监听套接字
    SOCKET listenSock = CreateAndBindSocket("8080");
    if (listenSock == INVALID_SOCKET) {
        WSACleanup();
        return 1;
    }

    // 开始监听
    if (listen(listenSock, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "listen failed: " << WSAGetLastError() << std::endl;
        closesocket(listenSock);
        WSACleanup();
        return 1;
    }

    std::cout << "Server listening on port 8080..." << std::endl;

    fd_set masterSet, readfds;
    FD_ZERO(&masterSet);
    FD_SET(listenSock, &masterSet);
    int maxFd = listenSock;

    while (true) {
        readfds = masterSet;

        // 使用select等待事件
        int activity = select(maxFd + 1, &readfds, NULL, NULL, NULL);
        if (activity == SOCKET_ERROR) {
            std::cerr << "select failed: " << WSAGetLastError() << std::endl;
            break;
        }

        // 检查监听套接字是否有新的连接请求
        if (FD_ISSET(listenSock, &readfds)) {
            sockaddr_in clientAddr;
            int clientAddrSize = sizeof(clientAddr);
            SOCKET clientSock = accept(listenSock, (sockaddr *) &clientAddr, &clientAddrSize);
            if (clientSock == INVALID_SOCKET) {
                std::cerr << "accept failed: " << WSAGetLastError() << std::endl;
                continue;
            }

            FD_SET(clientSock, &masterSet);
            if (clientSock > maxFd) {
                maxFd = clientSock;
            }

            std::cout << "New connection accepted, socket: " << clientSock << std::endl;
        }

        // 检查其他socket是否可读
        for (int i = 0; i <= maxFd; ++i) {
            SOCKET sock = (SOCKET) i;
            if (sock != listenSock && FD_ISSET(sock, &readfds)) {
                HandleClientData(sock);
            }
        }
    }

    // 清理资源
    for (int i = 0; i <= maxFd; ++i) {
        if (FD_ISSET((SOCKET)i, &masterSet)) {
            closesocket((SOCKET) i);
        }
    }
    closesocket(listenSock);
    WSACleanup();
    return 0;
}
