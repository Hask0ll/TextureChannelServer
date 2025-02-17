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
    }

    ~Application()
    {
        Renderer::Cleanup();
        Renderer::SetUpdateCallback([this] { this->Update(); });
        testTexture =  new Texture();
        testTexture->AddPerlinOperator(5);
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
