#pragma once
#include <list>

#include "Logger/Logger.h"
#include "Renderer/Renderer.h"
#include "Storage/Storage.h"
#include "Texture/Texture.h"
#include "UI/TextureManager.h"

class Application
{
public:
    Application()
    {
        Renderer::Init();
        Renderer::SetUpdateCallback([this] { this->Update(); });
        Logger::Init("app.log", Logger::Level::DEBUG);
        Logger::Info("Initialisation de l'application...");
        Logger::Info("Application démarrée");
        m_storage = std::make_shared<Storage>();
        m_TextureUI = std::make_unique<TextureManagerUI>();
        m_Instance = this;
    }

    ~Application()
    {
        Logger::Shutdown();
        Renderer::Cleanup();
    }

    static Application& Get()
    {
        return *m_Instance;
    }

    std::shared_ptr<Storage> GetStorage()
    {
        return m_storage;
    }

    void Run()
    {
        Renderer::Update();
    }

    void Update ()
    {
        m_TextureUI->Draw();
    }

    void AddTexture(std::string name)
    {
        
    }
private:
    std::list<unsigned char> m_stacks;
    std::shared_ptr<Storage> m_storage = nullptr;
    static inline Application* m_Instance = nullptr;
    std::unique_ptr<TextureManagerUI> m_TextureUI;
};
