#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <imgui.h>
#include <algorithm>

#include "Operator.h"

class PerlinNoiseOperator : public Operator {
public:
    virtual std::string GetOperatorName()
    {
        return "Perlin Noise";
    }
  
private:
    int m_Width, m_Height;
    GLuint m_TextureID;
    int m_OctavesNumber;

    std::vector<unsigned char> m_Pixels;

    

    float Generate2D(float x, float y) {
        int x0 = int(x);
        int y0 = int(y);
        int x1 = x0 + 1;
        int y1 = y0 + 1;
        
        float sx = x - (float)x0;
        float sy = y - (float)y0;
        
        float n0 = GenerateGradientDistanceDotProduct(x0, y0, x, y);
        float n1 = GenerateGradientDistanceDotProduct(x1, y0, x, y);
        float ix0 = CubicInterpolation(n0, n1, sx);
        
        n0 = GenerateGradientDistanceDotProduct(x0, y1, x, y);
        n1 = GenerateGradientDistanceDotProduct(x1, y1, x, y);
        float ix1 = CubicInterpolation(n0, n1, sx);
        
        return CubicInterpolation(ix0, ix1, sy);
    }

    float GenerateGradientDistanceDotProduct(int ix, int iy, float x, float y) {
        glm::vec2 gradient = GenerateRandomGradient(ix, iy);
        float dx = x - (float)ix;
        float dy = y - (float)iy;
        return (dx * gradient.x + dy * gradient.y);
    }

    glm::vec2 GenerateRandomGradient(int x, int y) {
        const unsigned w = 8 * sizeof(unsigned);
        const unsigned s = w / 2; 
        unsigned a = x, b = y;
        a *= 3284157443;
        b ^= a << s | a >> w - s;
        b *= 1911520717;
        a ^= b << s | b >> w - s;
        a *= 2048419325;
        
        float random = a * (3.14159265f / ~(~0u >> 1));
        return glm::vec2(sin(random), cos(random));
    }

    float CubicInterpolation(float a0, float a1, float w) {
        return (a1 - a0) * (3.0f - w * 2.0f) * w * w + a0;
    }

public:
    PerlinNoiseOperator(int numberOfOctaves = 1): m_OctavesNumber(numberOfOctaves), m_Width(512), m_Height(512) {
        generateNoiseTexture();
    }

    void generateNoiseTexture() {
        std::vector<unsigned char> m_Pixels(m_Width * m_Height * 4);
        const float GRID_SIZE = 100.0f;
        
        for(int y = 0; y < m_Height; y++) {
            for(int x = 0; x < m_Width; x++) {
                float val = 0.0f;
                float frequency = 1.0f;
                float amplitude = 1.0f;
                
                for(int i = 0; i < m_OctavesNumber; i++) {
                    val += Generate2D(x * frequency / GRID_SIZE, 
                                    y * frequency / GRID_SIZE) * amplitude;
                    frequency *= 2.0f;
                    amplitude *= 0.5f;
                }
                
                val = std::clamp(val * 1.2f, -1.0f, 1.0f);
                unsigned char color = static_cast<unsigned char>((val + 1.0f) * 127.5f);
                
                int idx = (y * m_Width + x) * 4;
                m_Pixels[idx] = color;     // R
                m_Pixels[idx + 1] = color; // G
                m_Pixels[idx + 2] = color; // B
                m_Pixels[idx + 3] = 255;   // A
            }
        }

        // Créer la texture OpenGL
        glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);
        glTextureStorage2D(m_TextureID, 1, GL_RGBA8, m_Width, m_Height);
        glTextureSubImage2D(m_TextureID, 0, 0, 0, m_Width, m_Height, GL_RGBA, GL_UNSIGNED_BYTE, m_Pixels.data());
        
        glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    virtual void Draw() override {
        // Obtenir l'espace disponible dans la fenêtre
        ImVec2 available = ImGui::GetContentRegionAvail();
    
        // Afficher l'image en utilisant tout l'espace disponible
        ImGui::Image((ImTextureID)(intptr_t)m_TextureID, available);
    
        // Bouton en bas à droite
        ImGui::SetCursorPos(ImVec2(available.x - 100, available.y - 30)); // Position pour le bouton
        if (ImGui::Button("Regenerate")) {
            generateNoiseTexture();
        }
    }

    virtual std::vector<unsigned char> GetData()
    {
        return m_Pixels;
    }

    ~PerlinNoiseOperator() {
        glDeleteTextures(1, &m_TextureID);
    }
};
