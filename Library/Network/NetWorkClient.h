#pragma once
#include <iostream>

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <functional>

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

    // Message handling
    void SendMessage(const std::string& message);
    std::string ReceiveMessage();

    // Async message handling
    void SetMessageCallback(std::function<void(const std::string&)> callback);
    void StartListening();
    void StopListening();
    void Sending(bool ApplicationRunning);
    void NetworkClient::SendBinaryData(const char* data, size_t size);
    std::vector<char> NetworkClient::ReceiveBinaryData();
private:
    void InitializeWinsock();
    void SetupSocket();
    void Cleanup();

    SOCKET clientSocket;
    bool connected;
    bool listening;
    std::string serverIP;
    std::string serverPort;
    std::function<void(const std::string&)> messageCallback;
};
