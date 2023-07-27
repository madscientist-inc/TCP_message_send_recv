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
    }

    SOCKET clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[1024];

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &(serverAddr.sin_addr));
    serverAddr.sin_port = htons(12345);

    result = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (result == SOCKET_ERROR) {
        std::cerr << "Connection failed: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "サーバーに接続しました。" << std::endl;

    while (true) {
        //std::cout << "メッセージを入力してください: ";
        std::string message = "server";
        //std::getline(std::cin, message);

        send(clientSocket, message.c_str(), static_cast<int>(message.length()), 0);

        if (message == "exit") {
            break;
        }

        memset(buffer, 0, sizeof(buffer));
        result = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (result > 0) {
            std::cout << "サーバー: " << buffer << std::endl;
        }
        else if (result == 0) {
            std::cout << "サーバーが接続を切断しました。" << std::endl;
            break;
        }
        else {
            std::cerr << "受信エラー: " << WSAGetLastError() << std::endl;
            break;
        }
        Sleep(1000);
    }

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}





