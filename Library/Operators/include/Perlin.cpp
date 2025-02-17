#include "Perlin.h"


float Perlin::Generate2D(float x, float y)
{
    // grid cells coordinates
    // top left
    int x0 = int(x);
    int y0 = int(y);

    // bottom right
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    // interpolation weights
    float sx = x - (float)x0;
    float sy = y - (float)y0;

    // Compute and interpolate top two corners
    float n0 = GenerateGradientDistanceDotProduct(x0, y0, x, y);
    float n1 = GenerateGradientDistanceDotProduct(x1, y0, x, y);
    float ix0 = CubicInterploation(n0, n1, sx);


    n0 = GenerateGradientDistanceDotProduct(x0, y1, x, y);
    n1 = GenerateGradientDistanceDotProduct(x1, y1, x, y);
    float ix1 = CubicInterploation(n0, n1, sx);

    float value = CubicInterploation(ix0, ix1, sy);

    return value;
}

float Perlin::GenerateGradientDistanceDotProduct(int ix, int iy, float x, float y)
{
    // Get gradient from integer coordinates
    glm::vec2 gradient = GenerateRandomGradient(ix, iy);
    // Compute the distance vector
    float dx = x - (float)ix;
    float dy = y - (float)iy;
     
    // Compute the dot-product
    return (dx * gradient.x + dy * gradient.y);
}


glm::vec2 Perlin::GenerateRandomGradient(int x, int y)
{
    // No precomputed gradients mean this works for any number of grid coordinates
    const unsigned w = 8 * sizeof(unsigned);
    const unsigned s = w / 2; 
    unsigned a = x, b = y;
    a *= 3284157443;
     
    b ^= a << s | a >> w - s;
    b *= 1911520717;
     
    a ^= b << s | b >> w - s;
    a *= 2048419325;
    float random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*Pi]
        
    // Create the vector from the angle
    glm::vec2 v;
    v.x = sin(random);
    v.y = cos(random);
     
    return v;
}


float Perlin::CubicInterploation(float a0, float a1, float w)
{
    return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
}

