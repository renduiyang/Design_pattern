#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <vector>
#include <string>

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
    // 使用addrinfo时配合getaddrinfo，使用for循环遍历所有可能的地址
    // 这个用法类似遍历链表,socket和bind函数
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
void HandleClientData(SOCKET clientSock, fd_set &masterSet, int &maxFd) {
    char buffer[1024];
    int n = recv(clientSock, buffer, sizeof(buffer) - 1, 0);
    if (n > 0) {
        buffer[n] = '\0';
        std::cout << "Received from client: " << buffer << std::endl;

        // 解析消息类型
        std::string message(buffer);
        size_t colonPos = message.find(':');
        if (colonPos != std::string::npos) {
            std::string messageType = message.substr(0, colonPos);
            std::string data = message.substr(colonPos + 1);

            if (messageType == "HEARTBEAT") {
                // 发送回显数据
                const char *ack = "ACK";
                send(clientSock, ack, strlen(ack), 0);
            } else if (messageType == "DATA") {
                // 处理其他数据
                std::cout << "Received data: " << data << std::endl;
                // 可以在这里添加更多的数据处理逻辑
            } else {
                std::cerr << "Unknown message type: " << messageType << std::endl;
            }
        } else {
            std::cerr << "Invalid message format: " << buffer << std::endl;
        }
    } else if (n == 0) {
        // 连接关闭
        std::cout << "Connection closed by client." << std::endl;
        closesocket(clientSock);
        FD_CLR(clientSock, &masterSet); // 从集合中移除
        if (clientSock == maxFd) {
            while (maxFd > 0 && !FD_ISSET(maxFd, &masterSet)) {
                --maxFd; // 更新最大文件描述符
            }
        }
    } else {
        // 接收错误
        std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
        closesocket(clientSock);
        FD_CLR(clientSock, &masterSet); // 从集合中移除
        if (clientSock == maxFd) {
            while (maxFd > 0 && !FD_ISSET(maxFd, &masterSet)) {
                --maxFd; // 更新最大文件描述符
            }
        }
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
    // 监听套接字的状态：一旦服务端调用了 listen(listenSock, backlog) 函数，listenSock 就进入了监听状态。
    // 这意味着它会持续监听新的连接请求，并将这些请求放入内核维护的连接队列中。
    if (listen(listenSock, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "listen failed: " << WSAGetLastError() << std::endl;
        closesocket(listenSock);
        WSACleanup();
        return 1;
    }

    std::cout << "Server listening on port 8080..." << std::endl;

    fd_set masterSet, readfds;
    FD_ZERO(&masterSet);
    // 添加套接字 listenSocket到masterSet中
    // 监听新连接：listenSock 是一个监听套接字，它用于监听客户端的连接请求。
    // 当 select() 检测到 listenSock 可读时，这意味着有新的连接请求到来。
    // 此时，服务器会调用 accept() 来接受这个连接。
    FD_SET(listenSock, &masterSet);
    // 设置当前maxFd 这个listenSock即为服务端本身,因此初始化maxFd为listenSock
    int maxFd = listenSock;

    while (true) {
        // 复制masterSet到readfds
        readfds = masterSet;

        // 使用select等待事件
        int activity = select(maxFd + 1, &readfds, nullptr, nullptr, nullptr);
        if (activity == SOCKET_ERROR) {
            int error = WSAGetLastError();
            std::cerr << "select failed: " << error << std::endl;

            // 根据错误代码决定如何处理
            if (error == WSAEINTR) {
                // 被信号中断
                std::cerr << "select was interrupted by a signal." << std::endl;
                continue; // 重新尝试
            } else if (error == WSAEINVAL) {
                // 参数无效
                std::cerr << "Invalid parameters for select." << std::endl;
                break; // 终止循环
            } else if (error == WSAENOTSOCK) {
                // 文件描述符不是有效的套接字
                std::cerr << "File descriptor is not a valid socket." << std::endl;
                break; // 终止循环
            } else {
                // 其他未知错误
                std::cerr << "Unknown error occurred in select." << std::endl;
                break; // 终止循环
            }
        }

        // 检查监听套接字是否有新的连接请求
        // int FD_ISSET(int fd, fd_set *set); 这个检查时是检查服务端有没有连上新的客户端
        // 如果 fd 文件描述符在 set 中被设置了，则 FD_ISSET 返回非零值（通常是1），否则返回0。
        // 如果 select() 返回并且 FD_ISSET(listenSock, &readfds) 为真，表示有新的连接请求到达。
        //服务器调用 accept(listenSock, ...) 来接受新的连接，并返回一个新的已连接套接字 newSock
        if (FD_ISSET(listenSock, &readfds)) {
            sockaddr_in clientAddr;
            int clientAddrSize = sizeof(clientAddr);
            SOCKET clientSock = accept(listenSock, (sockaddr *) &clientAddr, &clientAddrSize);
            if (clientSock == INVALID_SOCKET) {
                std::cerr << "accept failed: " << WSAGetLastError() << std::endl;
                continue;
            }

            //处理已连接的客户端：clientSock 是通过 accept() 从 listenSock 接受的新连接得到的已连接套接字。
            //每个 clientSock 代表一个已经建立的连接，可以用来与特定的客户端进行通信。
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
                try {
                    HandleClientData(sock, masterSet, maxFd);
                } catch (const std::exception &e) {
                    std::cerr << "Exception caught in HandleClientData: " << e.what() << std::endl;
                    closesocket(sock);
                    FD_CLR(sock, &masterSet);
                    if (sock == maxFd) {
                        while (maxFd > 0 && !FD_ISSET(maxFd, &masterSet)) {
                            --maxFd; // 更新最大文件描述符
                        }
                    }
                } catch (...) {
                    std::cerr << "Unknown exception caught in HandleClientData." << std::endl;
                    closesocket(sock);
                    FD_CLR(sock, &masterSet);
                    if (sock == maxFd) {
                        while (maxFd > 0 && !FD_ISSET(maxFd, &masterSet)) {
                            --maxFd; // 更新最大文件描述符
                        }
                    }
                }
            }
        }
    }

    // 清理资源
    for (int i = 0; i <= maxFd; ++i) {
        if (FD_ISSET((SOCKET)i, &masterSet)) {
            closesocket((SOCKET) i);
        }
    }
    printf("end");
    closesocket(listenSock);
    WSACleanup();
    system("pause");
    return 0;
}
