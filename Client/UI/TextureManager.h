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
    std::vector<std::shared_ptr<Texture>> m_textures;
    std::unique_ptr<OperatorManager> m_operatorManager;
public:

    TextureManagerUI()
    {
        m_operatorManager = std::make_unique<OperatorManager>();
    }
    void Draw() {
        // Configuration du panneau latéral
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(PANEL_WIDTH, ImGui::GetIO().DisplaySize.y), ImGuiCond_FirstUseEver);
        
        ImGui::Begin("Texture Manager", nullptr, 
                     ImGuiWindowFlags_NoMove | 
                     ImGuiWindowFlags_NoResize | 
                     ImGuiWindowFlags_NoCollapse);
        
        // Liste des textures
        if (ImGui::BeginChild("TextureList", ImVec2(0, -ImGui::GetFrameHeightWithSpacing() * 2), true)) {
            for (int i = 0; i < m_textures.size(); i++) {
                if (ImGui::Selectable(m_textures[i]->GetName().c_str(), m_selectedTexture == i)) {
                    m_selectedTexture = i;
                    m_operatorManager->SetCurrentTexture(m_textures[i]);
                }
                
                // Menu contextuel pour chaque texture
                if (ImGui::BeginPopupContextItem(m_textures[i]->GetName().c_str())) {
                    if (ImGui::MenuItem("Delete")) {
                        // TODO: Implémenter la suppression de texture
                        m_textures.erase(m_textures.begin() + i);
                        if (m_selectedTexture == i) m_selectedTexture = -1;
                    }
                    ImGui::EndPopup();
                }
            }
        }
        ImGui::EndChild();
        
        // Champ de texte pour le nom de la nouvelle texture
        ImGui::SetNextItemWidth(-1);
        ImGui::InputText("##texturename", m_newTextureName, sizeof(m_newTextureName));
        
        // Bouton pour créer une nouvelle texture
        if (ImGui::Button("Create Texture", ImVec2(-1, 0))) {
            if (strlen(m_newTextureName) > 0) {
                TextureInfo newTexture;
                newTexture.name = m_newTextureName;
                std::shared_ptr<Texture> texture = std::make_shared<Texture>(std::string(m_newTextureName));
                m_textures.push_back(texture);
                memset(m_newTextureName, 0, sizeof(m_newTextureName));
            }
        }
        
        ImGui::End();
        
        // Fenêtre principale (optionnelle, pour afficher la texture sélectionnée)
        ImGui::SetNextWindowPos(ImVec2(PANEL_WIDTH, 0), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x - PANEL_WIDTH, 
                                       ImGui::GetIO().DisplaySize.y), 
                                ImGuiCond_FirstUseEver);
        
        ImGui::Begin("Texture View", nullptr, 
                     ImGuiWindowFlags_NoMove | 
                     ImGuiWindowFlags_NoResize | 
                     ImGuiWindowFlags_NoCollapse);
        
        if (m_selectedTexture >= 0 && m_selectedTexture < m_textures.size()) {
            // TODO: Afficher la texture sélectionnée
            ImGui::Text("Selected texture: %s", m_textures[m_selectedTexture]->GetName().c_str());
            m_operatorManager->Draw();
        }

        ImGui::End();
    }
};
