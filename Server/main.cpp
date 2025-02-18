#include "Server.h"

int main(int argc, char** argv) {
    try {
        Server server;
        server.Run();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
