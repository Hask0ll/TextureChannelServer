#include "UI/include/TextureManager.h"
#include "../Library/Network/UI/NetworkUI.h"
#include "../Library/Network/Texture/TextureNetworkManager.h"

TextureManagerUI::TextureManagerUI()
{
    m_operatorManager = std::make_unique<OperatorManager>();
}

void TextureManagerUI::SetNetworkUI(NetworkUI* networkUI)
{
    m_networkUI = networkUI;
}

void TextureManagerUI::SetNetworkManager(std::shared_ptr<TextureNetworkManager> networkManager)
{
    m_networkManager = networkManager;
}

void TextureManagerUI::Draw() {
    const float windowHeight = ImGui::GetIO().DisplaySize.y;
    const float halfHeight = windowHeight / 2.0f;

    // Left top panel (texture list)
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(PANEL_WIDTH, halfHeight), ImGuiCond_Always);

    ImGui::Begin("Texture Manager", nullptr,
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBringToFrontOnFocus);

    // Texture list
    if (ImGui::BeginChild("TextureList", ImVec2(0, -ImGui::GetFrameHeightWithSpacing() * 2), true)) {
        // Local textures
        if (ImGui::TreeNodeEx("Local Textures", ImGuiTreeNodeFlags_DefaultOpen)) {
            for (int i = 0; i < m_textures.size(); i++) {
                if (ImGui::Selectable(m_textures[i]->GetName().c_str(), m_selectedTexture == i)) {
                    m_selectedTexture = i;
                    m_operatorManager->SetCurrentTexture(m_textures[i]);

                    // Update the network UI with the selected texture
                    if (m_networkUI) {
                        m_networkUI->SetCurrentTexture(m_textures[i]);
                    }
                }

                if (ImGui::BeginPopupContextItem(m_textures[i]->GetName().c_str())) {
                    if (ImGui::MenuItem("Delete")) {
                        m_textures.erase(m_textures.begin() + i);
                        if (m_selectedTexture == i) m_selectedTexture = -1;
                    }
                    ImGui::EndPopup();
                }
            }
            ImGui::TreePop();
        }

        // Remote textures (received from network)
        if (m_networkUI) {
            if (ImGui::TreeNodeEx("Remote Textures", ImGuiTreeNodeFlags_DefaultOpen)) {
                const auto& receivedTextures = m_networkUI->GetReceivedTextures();
                for (size_t i = 0; i < receivedTextures.size(); i++) {
                    bool isSelected = (m_selectedTexture == -1) && (i == 0);
                    if (ImGui::Selectable(receivedTextures[i]->GetName().c_str(), isSelected)) {
                        m_selectedTexture = -1;  // Mark as remote texture
                        m_operatorManager->SetCurrentTexture(receivedTextures[i]);
                    }

                    if (ImGui::BeginPopupContextItem(receivedTextures[i]->GetName().c_str())) {
                        if (ImGui::MenuItem("Import to Local")) {
                            // Add the remote texture to local textures
                            m_textures.push_back(receivedTextures[i]);
                            m_selectedTexture = static_cast<int>(m_textures.size() - 1);
                            m_operatorManager->SetCurrentTexture(m_textures[m_selectedTexture]);
                        }
                        ImGui::EndPopup();
                    }
                }
                ImGui::TreePop();
            }
        }
    }
    ImGui::EndChild();

    // New texture input and button
    ImGui::SetNextItemWidth(-1);
    ImGui::InputText("##texturename", m_newTextureName, sizeof(m_newTextureName));

    if (ImGui::Button("Create Texture", ImVec2(-1, 0))) {
        if (strlen(m_newTextureName) > 0) {
            std::shared_ptr<Texture> texture = std::make_shared<Texture>(std::string(m_newTextureName));
            texture->SetOnMessageCallback([this]
                {
                    this->Notify();
                });
            m_textures.push_back(texture);
            memset(m_newTextureName, 0, sizeof(m_newTextureName));
        }
    }

    ImGui::End();

    // Left bottom panel (operators)
    ImGui::SetNextWindowPos(ImVec2(0, halfHeight), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(PANEL_WIDTH, halfHeight), ImGuiCond_Always);

    ImGui::Begin("Operators", nullptr,
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBringToFrontOnFocus);

    if (m_selectedTexture >= 0 && m_selectedTexture < m_textures.size()) {
        m_operatorManager->Draw();
    }
    else if (m_networkUI) {
        // Handle remote texture selection
        const auto& receivedTextures = m_networkUI->GetReceivedTextures();
        if (!receivedTextures.empty()) {
            // Read-only message for remote textures
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f),
                "Remote textures are read-only.\nImport to local to modify.");
        }
    }

    ImGui::End();

    // Main texture view (right panel)
    float rightPanelWidth = ImGui::GetIO().DisplaySize.x - PANEL_WIDTH;
    ImGui::SetNextWindowPos(ImVec2(PANEL_WIDTH, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(rightPanelWidth, windowHeight), ImGuiCond_Always);

    ImGui::Begin("Texture View", nullptr,
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBringToFrontOnFocus);

    if (m_selectedTexture >= 0 && m_selectedTexture < m_textures.size()) {
        // Local texture view
        m_textures[m_selectedTexture]->Draw();
    }
    else if (m_networkUI) {
        // Remote texture view
        const auto& receivedTextures = m_networkUI->GetReceivedTextures();
        if (!receivedTextures.empty()) {
            receivedTextures[0]->Draw();
        }
    }

    ImGui::End();
}

void TextureManagerUI::Notify() {
    if (m_networkUI && m_networkManager) {
        // Share with connected clients when a texture is modified
        if (m_selectedTexture >= 0 && m_selectedTexture < m_textures.size()) {
            m_networkManager->ShareTexture(m_textures[m_selectedTexture]);
        }
    }
}