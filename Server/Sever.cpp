#include "Server.h"
#include <iostream>
#include <WS2tcpip.h>
#include <stdexcept>

#pragma comment(lib, "Ws2_32.lib")

Server::Server(const char* port) : running(true) {
    InitializeWinsock();
    SetupServerSocket(port);

    std::cout << "Listening..." << std::endl;
    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        closesocket(listenSocket);
        WSACleanup();
        throw std::runtime_error("listen failed");
    }

    FD_ZERO(&master);
    FD_SET(listenSocket, &master);
}

Server::~Server() {
    closesocket(listenSocket);
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
        WSACleanup();
        throw std::runtime_error("accept failed");
    }
    FD_SET(client, &master);
    std::cout << "server: new connection from " << client << std::endl;
    std::string welcomeMsg = "Welcome to the server!\n";
    send(client, welcomeMsg);
}

void Server::HandleClientMessage(SOCKET sock)
{
    char buffer[512];
    ZeroMemory(buffer, 512);
    int bytesReceived = recv(sock, buffer, 511, 0);

    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        std::string message(buffer);
        if (message.empty() || message == "\n" || message == "\r\n") {
            std::cout << "Empty message received, connection closing from client" << std::endl;
        }
        else {
            std::cout << "Message received (" << bytesReceived << " bytes): " << message << std::endl;
            for (unsigned int i = 0; i < master.fd_count; i++) {
                SOCKET outSock = master.fd_array[i];
                if (outSock != listenSocket && outSock != sock) {
                    send(outSock, message);
                }
            }
        }
    }
    else if (bytesReceived == 0) {
        std::cout << "Client logout" << std::endl;
        FD_CLR(sock, &master);
    }
    else {
        for (unsigned int i = 0; i < master.fd_count; i++) {
            SOCKET outSock = master.fd_array[i];
            if (outSock != listenSocket && outSock != sock) {
                std::string msg = "Client disconnected";
                send(outSock, msg);
            }
        }
        running = false;
    }
}

void Server::send(SOCKET client, const std::string& message) 
{
    ::send(client, message.c_str(), message.size() + 1, 0);
}

void Server::Run() 
{
    while (running) {
        fd_set copy = master;
        int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

        for (int i = 0; i < socketCount; i++) {
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