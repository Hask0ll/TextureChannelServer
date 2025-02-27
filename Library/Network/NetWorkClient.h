#pragma once
#include <iostream>

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <functional>
#include <thread>
#include <vector>

#pragma comment(lib, "ws2_32.lib")

class NetworkClient {
public:
    explicit NetworkClient(const char* ip = "127.0.0.1", const char* port = "13579");
    ~NetworkClient();

    // Deleted copy constructor and assignment operator
    NetworkClient(const NetworkClient&) = delete;
    NetworkClient& operator=(const NetworkClient&) = delete;

    // Connection management
    void Connect();
    void Disconnect();
    bool IsConnected() const;
    void SetClientName(const std::string& name);

    // Message handling
    void SendTextMessage(const std::string& message);
    void SendTextureData(const std::string& textureName, const std::vector<unsigned char>& data);
    void RequestTexture(const std::string& clientName, const std::string& textureName);

    // Callbacks
    void SetMessageCallback(std::function<void(const std::string&)> callback);
    void SetTextureReceivedCallback(std::function<void(const std::string&, const std::vector<unsigned char>&)> callback);

    // Async message handling
    void StartListening();
    void StopListening();

private:
    void InitializeWinsock();
    void SetupSocket();
    void Cleanup();

    // Message handling helpers
    void HandleTextMessage(size_t size);
    void HandleTextureData(size_t size);

    SOCKET clientSocket;
    bool connected;
    bool listening;
    std::string serverIP;
    std::string serverPort;
    std::string clientName;
    std::thread listenThread;
    std::function<void(const std::string&)> messageCallback;
    std::function<void(const std::string&, const std::vector<unsigned char>&)> textureReceivedCallback;
};