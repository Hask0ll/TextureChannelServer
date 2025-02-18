#pragma once
#include <list>

#include "Logger/Logger.h"
#include "Renderer/Renderer.h"
#include "Texture/Texture.h"

class Application
{
public:
    Application()
    {
        Renderer::Init();
        Renderer::SetUpdateCallback([this] { this->Update(); });
        Logger::Init("app.log", Logger::Level::DEBUG);
        Logger::Info("Application Initialisation...");
        Logger::Info("Application Started");
        testTexture =  new Texture();
        //// testTexture->AddPerlinOperator(5);
		testTexture->SetOnMessageCallback([](const std::string& message) {
			Logger::Info(message);
			});
        testTexture->AddColorizerOperator();
    }

    ~Application()
    {
        Logger::Shutdown();
        Renderer::Cleanup();
    }

    void Run()
    {
        Renderer::Update();
    }

    void Update ()
    {
        testTexture->Draw();
    }

    void AddTexture(std::string name)
    {
    }
private:
    std::list<unsigned char> m_stacks;
    Texture* testTexture = nullptr;
};
