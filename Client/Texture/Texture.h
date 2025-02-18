#pragma once
#include <list>
#include <vector>

#include "Operators/include/Colorizer.h"
// #include "Operators/include/Load.h"
#include "Operators/include/Operator.h"
#include "Operators/include/Perlin.h"
// #include "Operators/include/Save.h"

enum OperatorName
{
    Perlin,
    Colorizer,
};


class Texture
{
public:
    Texture(std::string& name): m_width(512), m_height(512), m_name(name) 
    {
    }

    void AddPerlinOperator(int octavesNumber)
    {
        auto perlin = new PerlinNoiseOperator(octavesNumber);
        m_operators.push_back(perlin);
        perlin->generateNoiseTexture(stack);
		// m_messageCallback("Perlin operator added");
    }

    void AddColorizerOperator(glm::vec3 color)
    {
        auto colorizer = new ColorizerOperator(color);
        colorizer->processTexture(stack);
        m_operators.push_back(colorizer);
		// m_messageCallback("Colorizer operator added");
    }

    void AddLoadOperator(std::string& name)
    {
        // auto load = new LoadOperator();
        // load->process(name);
        // stack.push_back(load->GetData());
        // m_operators.push_back(load);
    }

    void AddSaveOperator(std::string& name)
    {
        // auto data = stack.back();
        // auto save = new SaveOperator();
        // save->process(name, data);
        // stack.push_back(data);
        // m_operators.push_back(save);
    }

    std::string GetName()
    {
        return m_name;
    }

    void Draw()
    {
        if(m_operators.empty()) return;
        auto last = m_operators.back();
        if(last == nullptr) return;
        last->Draw();
    }

    std::vector<Operator*> GetOperators()
    {
        return m_operators;
    }
  
    void SetOnMessageCallback(std::function<void(const std::string&)> callback)
    {
        m_messageCallback = callback;
    }
    std::list<std::vector<unsigned char>> stack;
private:
    std::vector<Operator*> m_operators;
    int m_width, m_height;
    std::string m_name;
    std::function<void(const std::string&)> m_messageCallback;
};
