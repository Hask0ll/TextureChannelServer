#include "Application/Application.h"
#include "UI/include/TextureManager.h"
#include "../Library/Network/Texture/TextureNetworkManager.h"
#include "../Library/Network/UI/NetworkUI.h"

Application::Application()
{
    Renderer::Init();
    Renderer::SetUpdateCallback([this] { this->Update(); });
    m_storage = std::make_shared<Storage>();
    m_Instance = this;

    // Initialize network components
    m_Client = std::make_shared<NetworkClient>();
    m_NetworkManager = std::make_shared<TextureNetworkManager>(m_Client);
    m_NetworkUI = std::make_unique<NetworkUI>(m_NetworkManager);

    // Create and initialize texture UI
    m_TextureUI = std::make_unique<TextureManagerUI>();
    m_TextureUI->SetNetworkUI(m_NetworkUI.get());
    m_TextureUI->SetNetworkManager(m_NetworkManager);
}

Application::~Application()
{
    Renderer::Cleanup();
}

std::shared_ptr<Storage> Application::GetStorage()
{
    return m_storage;
}

std::shared_ptr<NetworkClient> Application::GetClient()
{
    return m_Client;
}

std::shared_ptr<TextureNetworkManager> Application::GetNetworkManager()
{
    return m_NetworkManager;
}

void Application::Run()
{
    Renderer::Update();
}

void Application::Update()
{
    m_TextureUI->Draw();
    m_NetworkUI->Draw();
}

void Application::AddTexture(std::string name)
{
    // Not implemented
}