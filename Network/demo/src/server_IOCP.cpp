#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#define MAX_BUFF_SIZE 1024

// 定义重叠结构体
typedef struct _PER_IO_OPERATION_DATA {
    WSAOVERLAPPED Overlapped;
    WSABUF DataBuf;
    CHAR Buffer[MAX_BUFF_SIZE];
    DWORD BytesSend;
    DWORD BytesRecv;
} PER_IO_OPERATION_DATA, *LPPER_IO_OPERATION_DATA;

int main() {
    // 初始化Windows Socket库
    WSAData wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cout << "WSAStartup failed: " << iResult << std::endl;
        return 1;
    }

    // 创建socket监听客户端连接
    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        std::cout << "socket failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(12345);

    // 绑定地址和端口
    iResult = bind(listenSocket, (sockaddr *) &serverAddr, sizeof(serverAddr));
    if (iResult == SOCKET_ERROR) {
        std::cout << "bind failed: " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // 开始监听
    iResult = listen(listenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        std::cout << "listen failed: " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // 创建IOCP句柄
    HANDLE iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    if (iocpHandle == NULL) {
        std::cout << "CreateIoCompletionPort failed: " << GetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // 将监听socket关联到IOCP上
    if (CreateIoCompletionPort((HANDLE) listenSocket, iocpHandle, 0, 0) == NULL) {
        std::cout << "CreateIoCompletionPort failed: " << GetLastError() << std::endl;
        CloseHandle(iocpHandle);
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server started." << std::endl;

    while (true) {
        sockaddr_in clientAddr;
        int addrLen = sizeof(clientAddr);

        // 接收客户端连接
        SOCKET clientSocket = accept(listenSocket, (sockaddr *) &clientAddr, &addrLen);
        if (clientSocket == INVALID_SOCKET) {
            std::cout << "accept failed: " << WSAGetLastError() << std::endl;
            continue;
        }

        // 将客户端socket关联到IOCP上
        HANDLE clientHandle = CreateIoCompletionPort((HANDLE) clientSocket, iocpHandle, 0, 0);
        if (clientHandle == NULL) {
            std::cout << "CreateIoCompletionPort failed: " << GetLastError() << std::endl;
            closesocket(clientSocket);
            continue;
        }

        // 创建重叠结构体
        LPPER_IO_OPERATION_DATA perIoData = new PER_IO_OPERATION_DATA();
        memset(&(perIoData->Overlapped), 0, sizeof(WSAOVERLAPPED));
        perIoData->DataBuf.len = MAX_BUFF_SIZE;
        perIoData->DataBuf.buf = perIoData->Buffer;

        DWORD bytesRecv = 0;
        DWORD flags = 0;


        Sleep(100);

        // 接收客户端数据
        if (WSARecv(clientSocket, &(perIoData->DataBuf), 1, &bytesRecv, &flags, &(perIoData->Overlapped), NULL) ==
            SOCKET_ERROR) {
            if (WSAGetLastError() != WSA_IO_PENDING) {
                std::cout << "WSARecv failed: " << WSAGetLastError() << std::endl;
                delete perIoData;
                closesocket(clientSocket);
                continue;
            }
        }
        std::cout << "WSARecv pending." << std::endl;
        printf("get data is %s \n", perIoData->Buffer);
    }

    CloseHandle(iocpHandle);
    closesocket(listenSocket);
    WSACleanup();

    return 0;
}
