#include <iostream>
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <stdexcept>

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

public:
    explicit Server(const char* port = "13579");
    ~Server();

    // Deleted copy constructor and assignment operator
    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;

    void send(SOCKET client, const std::string& message);
    void Run();
};