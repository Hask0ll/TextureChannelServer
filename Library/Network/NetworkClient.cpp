#include "NetWorkClient.h"
#include <WS2tcpip.h>
#include <stdexcept>
#include <thread>
#include <iostream>
#include <vector>
#include <functional>

#pragma comment(lib, "ws2_32.lib")

NetworkClient::NetworkClient(const char* ip, const char* port)
    : clientSocket(INVALID_SOCKET)
    , connected(false)
    , listening(false)
    , serverIP(ip)
    , serverPort(port)
    , clientName("Anonymous")
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

        // Set client name if not already set
        if (clientName == "Anonymous") {
            char hostname[256];
            if (gethostname(hostname, sizeof(hostname)) == 0) {
                clientName = hostname;
            }
        }

        // Send client name to server
        SendTextMessage("NAME:" + clientName);
    }
}

void NetworkClient::SetClientName(const std::string& name) {
    clientName = name;
    if (connected) {
        // Notify server about name change
        SendTextMessage("NAME:" + clientName);
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

void NetworkClient::SendTextMessage(const std::string& message) {
    if (!connected) {
        throw std::runtime_error("Not connected to server");
    }

    // Text message format
    int messageType = 0;
    size_t messageSize = message.size();

    char header[sizeof(int) + sizeof(size_t)];
    memcpy(header, &messageType, sizeof(int));
    memcpy(header + sizeof(int), &messageSize, sizeof(size_t));

    if (send(clientSocket, header, sizeof(header), 0) == SOCKET_ERROR) {
        throw std::runtime_error("Failed to send message header");
    }

    if (send(clientSocket, message.c_str(), message.size(), 0) == SOCKET_ERROR) {
        throw std::runtime_error("Failed to send message content");
    }
}

void NetworkClient::SendTextureData(const std::string& textureName, const std::vector<unsigned char>& data) {
    if (!connected) {
        throw std::runtime_error("Not connected to server");
    }

    // Texture message format
    int messageType = 1;
    size_t totalSize = 1 + textureName.size() + data.size();

    char header[sizeof(int) + sizeof(size_t)];
    memcpy(header, &messageType, sizeof(int));
    memcpy(header + sizeof(int), &totalSize, sizeof(size_t));

    if (send(clientSocket, header, sizeof(header), 0) == SOCKET_ERROR) {
        throw std::runtime_error("Failed to send texture header");
    }

    // Send name length and name
    char nameLength = static_cast<char>(textureName.size());
    if (send(clientSocket, &nameLength, 1, 0) == SOCKET_ERROR ||
        send(clientSocket, textureName.c_str(), textureName.size(), 0) == SOCKET_ERROR) {
        throw std::runtime_error("Failed to send texture name");
    }

    // Send texture data
    if (send(clientSocket, reinterpret_cast<const char*>(data.data()), data.size(), 0) == SOCKET_ERROR) {
        throw std::runtime_error("Failed to send texture data");
    }
}

void NetworkClient::RequestTexture(const std::string& clientName, const std::string& textureName) {
    if (!connected) {
        throw std::runtime_error("Not connected to server");
    }

    // Texture request format
    int messageType = 2;
    size_t totalSize = 1 + clientName.size() + 1 + textureName.size();

    char header[sizeof(int) + sizeof(size_t)];
    memcpy(header, &messageType, sizeof(int));
    memcpy(header + sizeof(int), &totalSize, sizeof(size_t));

    if (send(clientSocket, header, sizeof(header), 0) == SOCKET_ERROR) {
        throw std::runtime_error("Failed to send texture request header");
    }

    // Send client name length and name
    char clientNameLength = static_cast<char>(clientName.size());
    if (send(clientSocket, &clientNameLength, 1, 0) == SOCKET_ERROR ||
        send(clientSocket, clientName.c_str(), clientName.size(), 0) == SOCKET_ERROR) {
        throw std::runtime_error("Failed to send source client name");
    }

    // Send texture name length and name
    char textureNameLength = static_cast<char>(textureName.size());
    if (send(clientSocket, &textureNameLength, 1, 0) == SOCKET_ERROR ||
        send(clientSocket, textureName.c_str(), textureName.size(), 0) == SOCKET_ERROR) {
        throw std::runtime_error("Failed to send texture name");
    }
}

void NetworkClient::SetMessageCallback(std::function<void(const std::string&)> callback) {
    messageCallback = std::move(callback);
}

void NetworkClient::SetTextureReceivedCallback(std::function<void(const std::string&, const std::vector<unsigned char>&)> callback) {
    textureReceivedCallback = std::move(callback);
}

void NetworkClient::StartListening() {
    if (!connected || listening) {
        return;
    }

    listening = true;
    listenThread = std::thread([this]() {
        while (listening && connected) {
            try {
                // Receive message header (type and size)
                char headerBuffer[sizeof(int) + sizeof(size_t)];
                int bytesReceived = recv(clientSocket, headerBuffer, sizeof(headerBuffer), 0);

                if (bytesReceived <= 0) {
                    if (bytesReceived == 0) {
                        // Server disconnected
                        if (messageCallback) {
                            messageCallback("Disconnected from server");
                        }
                    }
                    else {
                        // Error
                        if (messageCallback) {
                            messageCallback("Error receiving from server: " + std::to_string(WSAGetLastError()));
                        }
                    }
                    connected = false;
                    listening = false;
                    break;
                }

                // Parse message header
                int messageType;
                size_t dataSize;
                memcpy(&messageType, headerBuffer, sizeof(int));
                memcpy(&dataSize, headerBuffer + sizeof(int), sizeof(size_t));

                // Handle based on message type
                switch (messageType) {
                case 0: // Text message
                    HandleTextMessage(dataSize);
                    break;
                case 1: // Texture data
                    HandleTextureData(dataSize);
                    break;
                default:
                    if (messageCallback) {
                        messageCallback("Received unknown message type: " + std::to_string(messageType));
                    }
                    break;
                }
            }
            catch (const std::exception& e) {
                if (messageCallback) {
                    messageCallback("Error: " + std::string(e.what()));
                }
                listening = false;
                connected = false;
                break;
            }
        }
        });

    listenThread.detach();
}

void NetworkClient::HandleTextMessage(size_t size) {
    std::vector<char> buffer(size);
    int bytesReceived = recv(clientSocket, buffer.data(), size, 0);

    if (bytesReceived <= 0) {
        throw std::runtime_error("Error receiving text message");
    }

    std::string message(buffer.data(), bytesReceived);

    if (messageCallback) {
        messageCallback(message);
    }
}

void NetworkClient::HandleTextureData(size_t size) {
    // Receive texture name length
    char nameLength;
    if (recv(clientSocket, &nameLength, 1, 0) <= 0) {
        throw std::runtime_error("Error receiving texture name length");
    }

    // Receive texture name
    std::vector<char> nameBuffer(nameLength);
    if (recv(clientSocket, nameBuffer.data(), nameLength, 0) <= 0) {
        throw std::runtime_error("Error receiving texture name");
    }
    std::string textureName(nameBuffer.data(), nameLength);

    // Receive texture data
    size_t dataSize = size - nameLength - 1;
    std::vector<unsigned char> textureData(dataSize);

    size_t totalReceived = 0;
    while (totalReceived < dataSize) {
        int bytesReceived = recv(clientSocket, reinterpret_cast<char*>(textureData.data() + totalReceived),
            dataSize - totalReceived, 0);
        if (bytesReceived <= 0) {
            throw std::runtime_error("Error receiving texture data");
        }
        totalReceived += bytesReceived;
    }

    // Notify about the received texture
    if (textureReceivedCallback) {
        textureReceivedCallback(textureName, textureData);
    }
}

void NetworkClient::StopListening() {
    listening = false;
    if (listenThread.joinable()) {
        listenThread.join();
    }
}

void NetworkClient::Cleanup() {
    WSACleanup();
}