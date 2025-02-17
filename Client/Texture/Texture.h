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
    }

    void AddColorizerOperator()
    {
        auto colorizer = new ColorizerOperator(m_width, m_height);
        colorizer->processTexture();
        m_stack.push_back(colorizer->GetData());
    }

    void Draw()
    {
        for(auto op : m_operators)
        {
            op->Draw();
        }
    }

private:
    std::list<std::vector<unsigned char>> m_stack;
    std::vector<Operator*> m_operators;
    int m_width, m_height;
};
