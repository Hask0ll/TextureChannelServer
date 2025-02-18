#pragma once
#include "Operator.h"
#include <glad/glad.h>
#include <imgui.h>
#include <glm/glm.hpp>
#include <vector>

class ColorizerOperator: public Operator
{
public:
    virtual std::string GetOperatorName() override
    {
        return "Colorizer";
    }

    ColorizerOperator(glm::vec3 color) : width(512), height(512) {
        // Initialiser avec du rouge comme couleur par défaut
        tintColor = ImVec4(color.x, color.y, color.z, 1.0f);

        // Créer la texture
        glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
        glTextureStorage2D(textureID, 1, GL_RGBA8, width, height);
        
        glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    ~ColorizerOperator() {
        glDeleteTextures(1, &textureID);
    }

    void processTexture(const std::vector<unsigned char>& input) {
        currentTexture.resize(width * height * 4);

        for (int i = 0; i < width * height; i++) {
            // Obtenir la luminosité depuis l'entrée (en utilisant le canal rouge)
            float brightness = input[i * 4] / 255.0f;
            
            // Appliquer la luminosité à la couleur de teinte
            currentTexture[i * 4 + 0] = static_cast<unsigned char>(brightness * tintColor.x * 255.0f);
            currentTexture[i * 4 + 1] = static_cast<unsigned char>(brightness * tintColor.y * 255.0f);
            currentTexture[i * 4 + 2] = static_cast<unsigned char>(brightness * tintColor.z * 255.0f);
            currentTexture[i * 4 + 3] = static_cast<unsigned char>(tintColor.w * 255.0f);
        }

        // Mettre à jour la texture
        glTextureSubImage2D(textureID, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, currentTexture.data());
    }

    void processTexture() {
        currentTexture.resize(width * height * 4);

        for (int i = 0; i < width * height; i++) {
            auto brightness = 1.f;
            
            // Appliquer la luminosité à la couleur de teinte
            currentTexture[i * 4 + 0] = static_cast<unsigned char>(brightness * tintColor.x * 255.0f);
            currentTexture[i * 4 + 1] = static_cast<unsigned char>(brightness * tintColor.y * 255.0f);
            currentTexture[i * 4 + 2] = static_cast<unsigned char>(brightness * tintColor.z * 255.0f);
            currentTexture[i * 4 + 3] = static_cast<unsigned char>(tintColor.w * 255.0f);
        }

        // Mettre à jour la texture
        glTextureSubImage2D(textureID, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, currentTexture.data());
    }

    void Draw() override {
        ImVec2 available = ImGui::GetContentRegionAvail();
        ImGui::Image((ImTextureID)(intptr_t)textureID, available);
    }

    virtual std::vector<unsigned char> GetData()
    {
        return currentTexture;
    }
private:
    int width, height;
    GLuint textureID;
    ImVec4 tintColor;
    std::vector<unsigned char> currentTexture;
};
