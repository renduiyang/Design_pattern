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
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Error at socket(): " << WSAGetLastError() << std::endl;
        WSACleanup();
        return -1;
    }

    // 设置服务器地址
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(5555); // 服务器端口
    inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr); // 服务器IP地址

    // 连接到服务器
    if (connect(clientSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "Connection Failed with error code : " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return -1;
    }

    // 发送消息
    const char *message = "Hello, Server!";
    int bytesSent = send(clientSocket, message, strlen(message), 0);
    if (bytesSent == SOCKET_ERROR) {
        std::cerr << "Send failed with error code : " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return -1;
    }

    // 接收回复
    char buffer[1024];
    int bytesReceived = recv(clientSocket, buffer, 1024, 0);
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        std::cout << "Message from server: " << buffer << std::endl;
    } else if (bytesReceived == 0) {
        std::cout << "Server disconnected" << std::endl;
    } else {
        std::cerr << "Recv failed with error code : " << WSAGetLastError() << std::endl;
    }

    // 关闭套接字
    closesocket(clientSocket);

    // 清理 Winsock
    WSACleanup();

    system("pause");
    return 0;
}
