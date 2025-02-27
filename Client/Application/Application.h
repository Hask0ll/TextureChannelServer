#pragma once
#include <list>

#include "Network/NetWorkClient.h"
#include "Renderer/Renderer.h"
#include "Storage/Storage.h"

// Forward declarations
class TextureManagerUI;
class TextureNetworkManager;
class NetworkUI;

class Application
{
public:
    Application();
    ~Application();

    static Application& Get()
    {
        return *m_Instance;
    }

    std::shared_ptr<Storage> GetStorage();
    std::shared_ptr<NetworkClient> GetClient();
    std::shared_ptr<TextureNetworkManager> GetNetworkManager();

    void Run();
    void Update();
    void AddTexture(std::string name);

private:
    std::list<unsigned char> m_stacks;
    std::shared_ptr<Storage> m_storage = nullptr;
    static inline Application* m_Instance = nullptr;
    std::unique_ptr<TextureManagerUI> m_TextureUI;
    std::shared_ptr<NetworkClient> m_Client;
    std::shared_ptr<TextureNetworkManager> m_NetworkManager;
    std::unique_ptr<NetworkUI> m_NetworkUI;
};