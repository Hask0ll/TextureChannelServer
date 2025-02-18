#pragma once
#include <imgui.h>
#include <list>
#include <vector>
#include <glad/glad.h>

#include "Operator.h"

class LoadOperator: public Operator
{
public:
    virtual std::string GetOperatorName() override
    {
        return "Load";
    }

    LoadOperator() = default;

    void process(std::string& name, std::list<std::vector<unsigned char>> stack);
  
    virtual size_t GetSerializedSize() const override
    {
        return 0;
    }
    virtual void Serialize(char* buffer) const override
    {
        
    }
    virtual void Deserialize(const char* buffer) override
    {
        
    }

    virtual void Draw(std::list<std::vector<unsigned char>>) override
    {
        ImVec2 available = ImGui::GetContentRegionAvail();
        ImGui::Image((ImTextureID)(intptr_t)m_TextureID, available);
    }
private:
    std::vector<unsigned char> m_data;
    GLuint m_TextureID;
};
