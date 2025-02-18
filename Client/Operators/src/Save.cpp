#include "Operators/include/Save.h"

#include "Application/Application.h"


void SaveOperator::process(std::string& name, std::list<std::vector<unsigned char>> stack)
{
    auto data = stack.back();
    Application::Get().GetStorage()->Save(name, data);

    glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);
    glTextureStorage2D(m_TextureID, 1, GL_RGBA8, 512, 512);
    glTextureSubImage2D(m_TextureID, 0, 0, 0, 512, 512, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
        
    glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}
