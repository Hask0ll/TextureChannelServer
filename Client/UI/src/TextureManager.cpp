#include "Application/Application.h"

void TextureManagerUI::Notify()
{
    for(auto texture : m_textures)
    {
        // NOT IMLEMENTED YET SORRY CHARLES
        // size_t textureSize = texture->GetSerialzedSize();
        // std::vector<char> buffer(textureSize);
        // Application::Get().GetClient()->SendBinaryData(buffer.data(), textureSize);
    }
}
