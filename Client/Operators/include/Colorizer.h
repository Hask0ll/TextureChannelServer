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
        tintColor = ImVec4(color.x, color.y, color.z, 1.0f);

        // create texture
        glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
        glTextureStorage2D(textureID, 1, GL_RGBA8, width, height);
        
        glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    ~ColorizerOperator() {
        glDeleteTextures(1, &textureID);
    }

void processTexture(std::list<std::vector<unsigned char>>& stack) {
    bool isStackEmpty = stack.empty();
    currentTexture.resize(width * height * 4);

    for (int i = 0; i < width * height; i++) {
        // Get existing color
        glm::vec4 existingColor;
        if (!isStackEmpty) {
            existingColor = glm::vec4(
                stack.back()[i * 4 + 0] / 255.0f,
                stack.back()[i * 4 + 1] / 255.0f,
                stack.back()[i * 4 + 2] / 255.0f,
                stack.back()[i * 4 + 3] / 255.0f
            );
        } else {
            existingColor = glm::vec4(1.0f);
        }

        // Here we blend the colors
        glm::vec4 finalColor = glm::vec4(
            fmin(existingColor.x + tintColor.x, 1.0f),
            fmin(existingColor.y + tintColor.y, 1.0f),
            fmin(existingColor.z + tintColor.z, 1.0f),
            tintColor.w * existingColor.w
         );
        
        currentTexture[i * 4 + 0] = static_cast<unsigned char>(finalColor.x * 255.0f);
        currentTexture[i * 4 + 1] = static_cast<unsigned char>(finalColor.y * 255.0f);
        currentTexture[i * 4 + 2] = static_cast<unsigned char>(finalColor.z * 255.0f);
        currentTexture[i * 4 + 3] = static_cast<unsigned char>(finalColor.w * 255.0f);
    }
    
    stack.pop_back();
    stack.push_back(currentTexture);
    
    glTextureSubImage2D(textureID, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, currentTexture.data());
}

    void Draw(std::list<std::vector<unsigned char>> stack) override {
        ImVec2 available = ImGui::GetContentRegionAvail();
        ImGui::Image((ImTextureID)(intptr_t)textureID, available);
    }

    virtual std::vector<unsigned char> GetData()
    {
        return currentTexture;
    }

    virtual size_t GetSerializedSize() const override {
        return sizeof(float) * 4;
    }

    virtual void Serialize(char* buffer) const override {
        size_t offset = 0;
        memcpy(buffer + offset, &tintColor.x, sizeof(float));
        offset += sizeof(float);
        memcpy(buffer + offset, &tintColor.y, sizeof(float));
        offset += sizeof(float);
        memcpy(buffer + offset, &tintColor.z, sizeof(float));
        offset += sizeof(float);
        memcpy(buffer + offset, &tintColor.w, sizeof(float));
    }

    virtual void Deserialize(const char* buffer) override {
        size_t offset = 0;
        memcpy(&tintColor.x, buffer + offset, sizeof(float));
        offset += sizeof(float);
        memcpy(&tintColor.y, buffer + offset, sizeof(float));
        offset += sizeof(float);
        memcpy(&tintColor.z, buffer + offset, sizeof(float));
        offset += sizeof(float);
        memcpy(&tintColor.w, buffer + offset, sizeof(float));
    }

private:
    int width, height;
    GLuint textureID;
    ImVec4 tintColor;
    std::vector<unsigned char> currentTexture;
};
