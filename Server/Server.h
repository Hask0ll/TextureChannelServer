#pragma once
#include <iostream>
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <stdexcept>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

class Server {
private:
    SOCKET listenSocket;
    fd_set master;
    bool running;

    void InitializeWinsock();
    void SetupServerSocket(const char* port);
    void HandleNewConnection();
    void HandleClientMessage(SOCKET sock);

    // Message handling methods
    void HandleTextMessage(SOCKET sock, size_t size);
    void HandleTextureData(SOCKET sock, size_t size);
    void HandleTextureRequest(SOCKET sock, size_t size);

    // Send methods
    void SendTextMessage(SOCKET client, const std::string& message);
    void BroadcastTextMessage(const std::string& message, SOCKET exclude);
    void SendTextureData(SOCKET client, const std::string& textureName, const std::vector<unsigned char>& data);
    void SendTextureList(SOCKET client);

public:
    explicit Server(const char* port = "13579");
    ~Server();

    // Deleted copy constructor and assignment operator
    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;

    void Run();
};