#include <vector>
#include <string>
#include <imgui.h>
#include <memory>

#include "OperatorManager.h"
#include "Texture/Texture.h"


class TextureManagerUI {
private:

    struct TextureInfo {
        std::string name;
        int id;
    };

    char m_newTextureName[256] = "";  // Buffer pour le nom de la nouvelle texture
    int m_selectedTexture = -1;
    const float PANEL_WIDTH = 250.0f;  // Largeur du panneau latéral
    const float PANEL_HEIGHT = 200.0f;  // Largeur du panneau latéral

    std::vector<std::shared_ptr<Texture>> m_textures;
    std::unique_ptr<OperatorManager> m_operatorManager;
public:

    TextureManagerUI()
    {
        m_operatorManager = std::make_unique<OperatorManager>();
    }

    void Draw() {
        const float windowHeight = ImGui::GetIO().DisplaySize.y;
        const float halfHeight = windowHeight / 2.0f;

        // Panneau de gauche supérieur (liste des textures)
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(PANEL_WIDTH, halfHeight), ImGuiCond_Always);
        
        ImGui::Begin("Texture Manager", nullptr, 
                     ImGuiWindowFlags_NoMove | 
                     ImGuiWindowFlags_NoResize | 
                     ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoBringToFrontOnFocus);
        
        // Liste des textures
        if (ImGui::BeginChild("TextureList", ImVec2(0, -ImGui::GetFrameHeightWithSpacing() * 2), true)) {
            for (int i = 0; i < m_textures.size(); i++) {
                if (ImGui::Selectable(m_textures[i]->GetName().c_str(), m_selectedTexture == i)) {
                    m_selectedTexture = i;
                    m_operatorManager->SetCurrentTexture(m_textures[i]);
                }
                
                if (ImGui::BeginPopupContextItem(m_textures[i]->GetName().c_str())) {
                    if (ImGui::MenuItem("Delete")) {
                        m_textures.erase(m_textures.begin() + i);
                        if (m_selectedTexture == i) m_selectedTexture = -1;
                    }
                    ImGui::EndPopup();
                }
            }
        }
        ImGui::EndChild();
        
        // Input et bouton en bas
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
        
        // Panneau des opérateurs (moitié inférieure gauche)
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

        ImGui::End();

        // Panneau de rendu de texture (à droite)
        ImGui::SetNextWindowPos(ImVec2(PANEL_WIDTH, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x - PANEL_WIDTH, 
                                       windowHeight), 
                                ImGuiCond_Always);

        ImGui::Begin("Texture View", nullptr, 
                     ImGuiWindowFlags_NoMove | 
                     ImGuiWindowFlags_NoResize | 
                     ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoBringToFrontOnFocus);

        if (m_selectedTexture >= 0 && m_selectedTexture < m_textures.size()) {
            // ImGui::Text("Selected texture: %s", m_textures[m_selectedTexture]->GetName().c_str());
            // Ici, ajoutez le code pour afficher le rendu de la texture
            m_textures[m_selectedTexture]->Draw(); //perlin render here
        }

        ImGui::End();
    }

    void Notify();

};
