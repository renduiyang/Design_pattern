//
// Created by renduiyang on 24-10-27.
//
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

// #pragma comment(lib, "ws2_32.lib")  // 链接到 Winsock 库

int main() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return -1;
    }

    // 创建套接字
    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        std::cerr << "Error at socket(): " << WSAGetLastError() << std::endl;
        WSACleanup();
        return -1;
    }

    // 绑定地址到套接字
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY); // 监听所有接口
    serverAddress.sin_port = htons(5555); // 设置端口

    if (bind(listenSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "Bind failed with error code : " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return -1;
    }

    // 开始监听
    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        // SOMAXCONN 是推荐的最大连接数
        std::cerr << "Listen failed with error code : " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return -1;
    }

    std::cout << "Server is listening on port 5555..." << std::endl;

    // 接受客户端连接
    SOCKET clientSocket = accept(listenSocket, NULL, NULL);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Accept failed with error code : " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return -1;
    }

    std::cout << "Client connected." << std::endl;

    // 接收客户端消息
    char buffer[1024];
    int bytesReceived = recv(clientSocket, buffer, 1024, 0);
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        std::cout << "Message from client: " << buffer << std::endl;

        // 发送回复
        const char *response = "Hello, Client!";
        int bytesSent = send(clientSocket, response, strlen(response), 0);
        if (bytesSent == SOCKET_ERROR) {
            std::cerr << "Send failed with error code : " << WSAGetLastError() << std::endl;
        } else {
            std::cout << "Message sent to client: " << response << std::endl;
        }
    } else if (bytesReceived == 0) {
        std::cout << "Client disconnected" << std::endl;
    } else {
        std::cerr << "Recv failed with error code : " << WSAGetLastError() << std::endl;
    }

    // 关闭套接字
    closesocket(clientSocket);
    closesocket(listenSocket);

    // 清理 Winsock
    WSACleanup();

    system("pause");

    return 0;
}
