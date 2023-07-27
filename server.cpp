#include <iostream>
#include <cstring>
#include <WinSock2.h>
#include <windows.h>
#include <Ws2tcpip.h> // inet_ntopを利用するために必要な追加のヘッダー
#include <iostream>
#include <cstring>
#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return 1;
    }

    SOCKET serverSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    char buffer[1024];

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    result = bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (result == SOCKET_ERROR) {
        std::cerr << "Binding failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    result = listen(serverSocket, 1);
    if (result == SOCKET_ERROR) {
        std::cerr << "Listening failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "サーバーが起動しました。クライアントからの接続を待機中..." << std::endl;

    SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Accepting connection failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
    std::cout << "クライアントが接続しました。IPアドレス: " << clientIP << ", ポート: " << ntohs(clientAddr.sin_port) << std::endl;

    while (true) {
        //std::cout << "メッセージを入力してください: ";
        std::string message = "client";
        //std::getline(std::cin, message);

        send(clientSocket, message.c_str(), static_cast<int>(message.length()), 0);

        if (message == "exit") {
            break;
        }

        memset(buffer, 0, sizeof(buffer));
        result = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (result > 0) {
            std::cout << "クライアント: " << buffer << std::endl;
        }
        else if (result == 0) {
            std::cout << "クライアントが接続を切断しました。" << std::endl;
            break;
        }
        else {
            std::cerr << "受信エラー: " << WSAGetLastError() << std::endl;
            break;
        }
        Sleep(1000);
    }

    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}