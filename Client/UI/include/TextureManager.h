#pragma once
#include <imgui.h>
#include <memory>
#include <string>
#include <vector>

#include "OperatorManager.h"
#include "Texture/Texture.h"

// Forward declarations
class NetworkUI;
class TextureNetworkManager;

class TextureManagerUI {
private:
    struct TextureInfo {
        std::string name;
        int id;
    };

    char m_newTextureName[256] = "";  // Buffer for new texture name
    int m_selectedTexture = -1;
    const float PANEL_WIDTH = 250.0f;  // Side panel width
    const float PANEL_HEIGHT = 200.0f;  // Panel height

    std::vector<std::shared_ptr<Texture>> m_textures;
    std::unique_ptr<OperatorManager> m_operatorManager;
    NetworkUI* m_networkUI = nullptr;  // Added network UI reference
    std::shared_ptr<TextureNetworkManager> m_networkManager = nullptr;

public:
    TextureManagerUI();
    ~TextureManagerUI() = default;

    void SetNetworkUI(NetworkUI* networkUI);
    void SetNetworkManager(std::shared_ptr<TextureNetworkManager> networkManager);
    void Draw();
    void Notify();
};