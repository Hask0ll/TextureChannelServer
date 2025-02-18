#include "NetworkClient.h"
#include <WS2tcpip.h>
#include <stdexcept>
#include <thread>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

NetworkClient::NetworkClient(const char* ip, const char* port)
    : clientSocket(INVALID_SOCKET)
    , connected(false)
    , listening(false)
    , serverIP(ip)
    , serverPort(port)
{
    InitializeWinsock();
}

NetworkClient::~NetworkClient() {
    if (connected) {
        Disconnect();
    }
    Cleanup();
}

void NetworkClient::InitializeWinsock() {
    WSADATA wsaData;
    int err = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (err) {
        throw std::runtime_error("WSAStartup failed");
    }
}

void NetworkClient::SetupSocket() {
    struct addrinfo* result = nullptr;
    struct addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    int err = getaddrinfo(serverIP.c_str(), serverPort.c_str(), &hints, &result);
    if (err) {
        Cleanup();
        throw std::runtime_error("getaddrinfo failed");
    }

    clientSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (clientSocket == INVALID_SOCKET) {
        freeaddrinfo(result);
        Cleanup();
        throw std::runtime_error("socket failed");
    }

    err = ::connect(clientSocket, result->ai_addr, static_cast<int>(result->ai_addrlen));
    freeaddrinfo(result);

    if (err == SOCKET_ERROR) {
        closesocket(clientSocket);
        Cleanup();
        throw std::runtime_error("connect failed");
    }
}

void NetworkClient::Connect() {
    if (!connected) {
        SetupSocket();
        connected = true;
    }
}

void NetworkClient::Disconnect() {
    if (connected) {
        StopListening();
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
        connected = false;
    }
}

bool NetworkClient::IsConnected() const {
    return connected;
}

void NetworkClient::SendMessage(const std::string& message) {
    if (!connected) {
        throw std::runtime_error("Not connected to server");
    }

    int bytesSent = send(clientSocket, message.c_str(), message.length(), 0);
    if (bytesSent == SOCKET_ERROR) {
        throw std::runtime_error("Failed to send message");
    }
}

std::string NetworkClient::ReceiveMessage() {
    if (!connected) {
        throw std::runtime_error("Not connected to server");
    }

    char buffer[512];
    ZeroMemory(buffer, sizeof(buffer));

    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        return std::string(buffer);
    }
    else if (bytesReceived == 0) {
        Disconnect();
        throw std::runtime_error("Server disconnected");
    }
    else {
        throw std::runtime_error("Error receiving message");
    }
}

void NetworkClient::SetMessageCallback(std::function<void(const std::string&)> callback) {
    messageCallback = std::move(callback);
}

void NetworkClient::StartListening() {
    if (!connected || listening) {
        return;
    }

    listening = true;
    std::thread([this]() {
        while (listening && connected) {
            try {
                std::string message = ReceiveMessage();
                if (messageCallback) {
                    messageCallback(message);
                }
            }
            catch (const std::exception& e) {
                if (messageCallback) {
                    messageCallback("Error: " + std::string(e.what()));
                }
                listening = false;
                break;
            }
        }
        }).detach();
}

void NetworkClient::StopListening() {
    listening = false;
}

void NetworkClient::Cleanup() {
    WSACleanup();
}

void NetworkClient::Sending(bool ApplicationRunning)
{
    std::thread([&]() {
        bool retry = true;
        while (true)
        {
            std::string message;
            std::cout << "Client waiting message...";

            try {
                SendMessage(message);
            }
            catch (const std::exception& e) {
                std::cerr << "Error sending message: " << e.what() << std::endl;
                ApplicationRunning = false;
            }
        }
        }).detach();
}
