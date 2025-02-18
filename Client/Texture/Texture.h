#pragma once
#include <list>
#include <vector>

#include "Operators/include/Colorizer.h"
#include "Operators/include/Operator.h"
#include "Operators/include/Perlin.h"

enum OperatorName
{
    Perlin,
    Colorizer,
};


class Texture
{
public:
    Texture(): m_width(512), m_height(512)
    {
    }

    void AddPerlinOperator(int octavesNumber)
    {
        auto perlin = new PerlinNoiseOperator(octavesNumber);
        m_operators.push_back(perlin);
        perlin->generateNoiseTexture();
        m_stack.push_back(perlin->GetData());
		m_messageCallback("Perlin operator added");
    }

    void AddColorizerOperator()
    {
        auto colorizer = new ColorizerOperator(m_width, m_height);
        colorizer->processTexture();
        m_stack.push_back(colorizer->GetData());
        m_operators.push_back(colorizer);
		m_messageCallback("Colorizer operator added");
    }

    void Draw()
    {
        for(auto op : m_operators) // draw only stack last item
        {
            op->Draw();
        }
    }

    void SetOnMessageCallback(std::function<void(const std::string&)> callback)
    {
        m_messageCallback = callback;
    }

private:
    std::list<std::vector<unsigned char>> m_stack;
    std::vector<Operator*> m_operators;
    int m_width, m_height;
    std::function<void(const std::string&)> m_messageCallback;
};
