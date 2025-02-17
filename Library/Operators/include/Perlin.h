#pragma once
#include "Operator.h"
#include <glm/vec2.hpp>
class Perlin: public Operator
{
public:
    Perlin() = default;
    
    virtual std::string GetOperatorName() override
    {
        return "Perlin";
    }
    
private: 
    // Perlin noise implementation
    // ...

    float Generate2D(float x, float y);

    float GenerateGradientDistanceDotProduct(int ix, int iy, float x, float y);


    glm::vec2 GenerateRandomGradient(int ix, int iy);

    float CubicInterploation(float a0, float a1, float w);
};
