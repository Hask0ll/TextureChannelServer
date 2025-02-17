#pragma once
#include <list>

#include "Renderer/Renderer.h"
#include "Texture/Texture.h"

class Application
{
public:
    Application()
    {
        Renderer::Init();
        Renderer::SetUpdateCallback([this] { this->Update(); });
        testTexture =  new Texture();
        // testTexture->AddPerlinOperator(5);
        testTexture->AddColorizerOperator();
    }

    ~Application()
    {
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
