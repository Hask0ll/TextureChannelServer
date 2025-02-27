#include "Server.h"
#include <iostream>
#include <WS2tcpip.h>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

// Structure to store client information
struct ClientInfo {
    std::string name;
    std::unordered_map<std::string, std::vector<unsigned char>> textures;
};

// Global client registry
std::unordered_map<SOCKET, ClientInfo> clientRegistry;

Server::Server(const char* port) : running(true) {
    InitializeWinsock();
    SetupServerSocket(port);

    std::cout << "Listening on port " << port << "..." << std::endl;
    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        closesocket(listenSocket);
        WSACleanup();
        throw std::runtime_error("listen failed");
    }

    FD_ZERO(&master);
    FD_SET(listenSocket, &master);
}

Server::~Server() {
    for (unsigned int i = 0; i < master.fd_count; i++) {
        SOCKET sock = master.fd_array[i];
        closesocket(sock);
    }
    WSACleanup();
}

void Server::InitializeWinsock() {
    WSADATA wsaData;
    int err = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (err)
        throw std::runtime_error("WSAStartup failed");
}

void Server::SetupServerSocket(const char* port)
{
    struct addrinfo* result = nullptr;
    struct addrinfo hints;
    ZeroMemory(&hints, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    int err = getaddrinfo(nullptr, port, &hints, &result);
    if (err) {
        WSACleanup();
        throw std::runtime_error("getaddrinfo failed");
    }

    listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (listenSocket == INVALID_SOCKET) {
        freeaddrinfo(result);
        WSACleanup();
        throw std::runtime_error("socket failed");
    }

    // Set socket to non-blocking mode
    u_long mode = 1;
    ioctlsocket(listenSocket, FIONBIO, &mode);

    err = bind(listenSocket, result->ai_addr, int(result->ai_addrlen));
    freeaddrinfo(result);
    if (err) {
        closesocket(listenSocket);
        WSACleanup();
        throw std::runtime_error("bind failed");
    }
}

void Server::HandleNewConnection()
{
    SOCKET client = accept(listenSocket, nullptr, nullptr);
    if (client == INVALID_SOCKET) {
        if (WSAGetLastError() != WSAEWOULDBLOCK) {
            std::cerr << "accept failed: " << WSAGetLastError() << std::endl;
        }
        return;
    }

    // Add new client to the master set
    FD_SET(client, &master);

    // Initialize client info
    clientRegistry[client] = ClientInfo();
    clientRegistry[client].name = "Client_" + std::to_string(client);

    std::cout << "New connection from " << clientRegistry[client].name << " (socket: " << client << ")" << std::endl;

    // Send welcome message
    std::string welcomeMsg = "WELCOME:" + clientRegistry[client].name;
    SendTextMessage(client, welcomeMsg);

    // Broadcast connection notification to other clients
    std::string connectMsg = "CONNECT:" + clientRegistry[client].name;
    BroadcastTextMessage(connectMsg, client);

    // Send list of available textures to new client
    SendTextureList(client);
}

void Server::HandleClientMessage(SOCKET sock)
{
    // Buffer for receiving message header (message type and size)
    char headerBuffer[sizeof(int) + sizeof(size_t)];
    int bytesReceived = recv(sock, headerBuffer, sizeof(headerBuffer), 0);

    if (bytesReceived <= 0) {
        // Handle disconnection
        if (bytesReceived == 0) {
            std::cout << "Client " << clientRegistry[sock].name << " disconnected" << std::endl;
        }
        else {
            std::cerr << "Error receiving from client: " << WSAGetLastError() << std::endl;
        }

        // Clean up client resources
        std::string disconnectMsg = "DISCONNECT:" + clientRegistry[sock].name;
        clientRegistry.erase(sock);
        FD_CLR(sock, &master);
        closesocket(sock);

        // Inform other clients
        BroadcastTextMessage(disconnectMsg, INVALID_SOCKET);
        return;
    }

    // Process the message based on its type
    int messageType;
    size_t dataSize;
    memcpy(&messageType, headerBuffer, sizeof(int));
    memcpy(&dataSize, headerBuffer + sizeof(int), sizeof(size_t));

    switch (messageType) {
    case 0: // Text message
        HandleTextMessage(sock, dataSize);
        break;
    case 1: // Texture data
        HandleTextureData(sock, dataSize);
        break;
    case 2: // Texture request
        HandleTextureRequest(sock, dataSize);
        break;
    default:
        std::cerr << "Unknown message type: " << messageType << std::endl;
        break;
    }
}

void Server::HandleTextMessage(SOCKET sock, size_t size) {
    // Receive the actual message
    std::vector<char> buffer(size);
    int bytesReceived = recv(sock, buffer.data(), size, 0);

    if (bytesReceived <= 0) {
        std::cerr << "Error receiving text message" << std::endl;
        return;
    }

    std::string message(buffer.data(), bytesReceived);
    std::cout << "Text message from " << clientRegistry[sock].name << ": " << message << std::endl;

    // Process commands
    if (message.substr(0, 5) == "NAME:") {
        // Client is changing their name
        std::string oldName = clientRegistry[sock].name;
        clientRegistry[sock].name = message.substr(5);
        std::cout << "Client " << oldName << " renamed to " << clientRegistry[sock].name << std::endl;

        // Notify all clients of the name change
        std::string renameMsg = "RENAME:" + oldName + ":" + clientRegistry[sock].name;
        BroadcastTextMessage(renameMsg, INVALID_SOCKET);
    }
    else {
        // Regular chat message, broadcast to all clients
        std::string chatMsg = "CHAT:" + clientRegistry[sock].name + ":" + message;
        BroadcastTextMessage(chatMsg, INVALID_SOCKET);
    }
}

void Server::HandleTextureData(SOCKET sock, size_t size) {
    // Header: texture name length + texture name + texture data
    char nameLength;
    if (recv(sock, &nameLength, 1, 0) <= 0) {
        std::cerr << "Error receiving texture name length" << std::endl;
        return;
    }

    // Get texture name
    std::vector<char> nameBuffer(nameLength);
    if (recv(sock, nameBuffer.data(), nameLength, 0) <= 0) {
        std::cerr << "Error receiving texture name" << std::endl;
        return;
    }
    std::string textureName(nameBuffer.data(), nameLength);

    // Get texture data
    size_t dataSize = size - nameLength - 1;
    std::vector<unsigned char> textureData(dataSize);

    // Receive the texture data in chunks
    size_t totalReceived = 0;
    while (totalReceived < dataSize) {
        int bytesReceived = recv(sock, reinterpret_cast<char*>(textureData.data() + totalReceived),
            dataSize - totalReceived, 0);
        if (bytesReceived <= 0) {
            std::cerr << "Error receiving texture data: " << WSAGetLastError() << std::endl;
            return;
        }
        totalReceived += bytesReceived;
    }

    // Store the texture in the client's registry
    clientRegistry[sock].textures[textureName] = textureData;

    std::cout << "Received texture '" << textureName << "' from " << clientRegistry[sock].name
        << " (" << dataSize << " bytes)" << std::endl;

    // Notify all clients about the new texture
    std::string textureMsg = "TEXTURE_AVAILABLE:" + clientRegistry[sock].name + ":" + textureName;
    BroadcastTextMessage(textureMsg, INVALID_SOCKET);
}

void Server::HandleTextureRequest(SOCKET sock, size_t size) {
    // Get request details: source client + texture name
    char sourceClientLength;
    if (recv(sock, &sourceClientLength, 1, 0) <= 0) {
        std::cerr << "Error receiving source client length" << std::endl;
        return;
    }

    std::vector<char> sourceBuffer(sourceClientLength);
    if (recv(sock, sourceBuffer.data(), sourceClientLength, 0) <= 0) {
        std::cerr << "Error receiving source client name" << std::endl;
        return;
    }
    std::string sourceClientName(sourceBuffer.data(), sourceClientLength);

    char textureNameLength;
    if (recv(sock, &textureNameLength, 1, 0) <= 0) {
        std::cerr << "Error receiving texture name length" << std::endl;
        return;
    }

    std::vector<char> nameBuffer(textureNameLength);
    if (recv(sock, nameBuffer.data(), textureNameLength, 0) <= 0) {
        std::cerr << "Error receiving texture name" << std::endl;
        return;
    }
    std::string textureName(nameBuffer.data(), textureNameLength);

    // Find the source client socket
    SOCKET sourceSocket = INVALID_SOCKET;
    for (const auto& [clientSocket, info] : clientRegistry) {
        if (info.name == sourceClientName) {
            sourceSocket = clientSocket;
            break;
        }
    }

    if (sourceSocket == INVALID_SOCKET) {
        std::cerr << "Source client not found: " << sourceClientName << std::endl;
        return;
    }

    // Check if the texture exists
    if (clientRegistry[sourceSocket].textures.find(textureName) == clientRegistry[sourceSocket].textures.end()) {
        std::cerr << "Texture not found: " << textureName << std::endl;
        return;
    }

    // Send the texture data to the requesting client
    const auto& textureData = clientRegistry[sourceSocket].textures[textureName];
    SendTextureData(sock, textureName, textureData);

    std::cout << "Sent texture '" << textureName << "' from " << sourceClientName
        << " to " << clientRegistry[sock].name << std::endl;
}

void Server::SendTextMessage(SOCKET client, const std::string& message) {
    // Send message type (0 for text) and size
    int messageType = 0;
    size_t messageSize = message.size();

    char header[sizeof(int) + sizeof(size_t)];
    memcpy(header, &messageType, sizeof(int));
    memcpy(header + sizeof(int), &messageSize, sizeof(size_t));

    if (send(client, header, sizeof(header), 0) == SOCKET_ERROR) {
        std::cerr << "Error sending message header: " << WSAGetLastError() << std::endl;
        return;
    }

    // Send actual message
    if (send(client, message.c_str(), message.size(), 0) == SOCKET_ERROR) {
        std::cerr << "Error sending message: " << WSAGetLastError() << std::endl;
    }
}

void Server::BroadcastTextMessage(const std::string& message, SOCKET exclude) {
    for (unsigned int i = 0; i < master.fd_count; i++) {
        SOCKET sock = master.fd_array[i];
        if (sock != listenSocket && sock != exclude) {
            SendTextMessage(sock, message);
        }
    }
}

void Server::SendTextureData(SOCKET client, const std::string& textureName, const std::vector<unsigned char>& data) {
    // Message type 1 (texture data)
    int messageType = 1;
    // Total size: name length (1 byte) + name + data
    size_t totalSize = 1 + textureName.size() + data.size();

    // Send header
    char header[sizeof(int) + sizeof(size_t)];
    memcpy(header, &messageType, sizeof(int));
    memcpy(header + sizeof(int), &totalSize, sizeof(size_t));

    if (send(client, header, sizeof(header), 0) == SOCKET_ERROR) {
        std::cerr << "Error sending texture header: " << WSAGetLastError() << std::endl;
        return;
    }

    // Send name length and name
    char nameLength = static_cast<char>(textureName.size());
    if (send(client, &nameLength, 1, 0) == SOCKET_ERROR ||
        send(client, textureName.c_str(), textureName.size(), 0) == SOCKET_ERROR) {
        std::cerr << "Error sending texture name: " << WSAGetLastError() << std::endl;
        return;
    }

    // Send texture data
    if (send(client, reinterpret_cast<const char*>(data.data()), data.size(), 0) == SOCKET_ERROR) {
        std::cerr << "Error sending texture data: " << WSAGetLastError() << std::endl;
    }
}

void Server::SendTextureList(SOCKET client) {
    // Create a list of all available textures
    std::string textureList = "TEXTURE_LIST:";

    for (const auto& [clientSocket, info] : clientRegistry) {
        if (clientSocket == client) continue; // Skip the client's own textures

        for (const auto& [textureName, _] : info.textures) {
            textureList += info.name + ":" + textureName + ";";
        }
    }

    // Send the list to the client
    if (!textureList.empty() && textureList != "TEXTURE_LIST:") {
        SendTextMessage(client, textureList);
    }
}

void Server::Run() {
    // Non-blocking select with timeout
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100000;  // 100ms timeout

    while (running) {
        fd_set copy = master;
        int socketCount = select(0, &copy, nullptr, nullptr, &tv);

        if (socketCount == SOCKET_ERROR) {
            std::cerr << "select failed: " << WSAGetLastError() << std::endl;
            running = false;
            break;
        }

        // Handle socket events
        for (unsigned int i = 0; i < copy.fd_count; i++) {
            SOCKET sock = copy.fd_array[i];

            if (sock == listenSocket) {
                HandleNewConnection();
            }
            else {
                HandleClientMessage(sock);
            }
        }
    }
}