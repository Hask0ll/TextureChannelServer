#pragma once
#include <functional>
#include <imgui.h>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

#include "Operators/include/Operator.h"
#include "Operators/include/Perlin.h"
#include "Texture/Texture.h"


enum OperatorType
{
    PerlinType,
    ColorizerType,
};

struct OperatorParams {
    virtual ~OperatorParams() = default;
    virtual OperatorType GetType() = 0;
};

struct PerlinNoiseParams : public OperatorParams {
public:
    int octaves = 4;
    OperatorType GetType() override { return OperatorType::PerlinType; }
};

class OperatorManager
{
    struct OperatorInfo {
        std::string name;
        std::function<std::shared_ptr<Operator>(std::shared_ptr<OperatorParams>)> factory;
        std::function<std::shared_ptr<OperatorParams>()> createDefaultParams;
        std::function<void(OperatorParams*)> drawParamsUI;
    };
    
public:

    OperatorManager()
    {
        registerPerlinNoiseOperator();
    }

    void SetCurrentTexture(std::shared_ptr<Texture> texture)
    {
        m_currentTexture = texture;
    }

    void registerPerlinNoiseOperator()
    {
        m_operators["Perlin Noise"] = {
            "Perlin Noise",
            [](std::shared_ptr<OperatorParams> params) {
                auto* perlinParams = dynamic_cast<PerlinNoiseParams*>(params.get());
                if (!perlinParams) {
                    return std::unique_ptr<Operator>(nullptr);  // Retour explicite d'un unique_ptr vide
                }
    
                return std::unique_ptr<Operator>(new PerlinNoiseOperator(perlinParams->octaves));
            },
            []() {
                return std::make_shared<PerlinNoiseParams>();
            },
            [](OperatorParams* params) {
                auto perlinParams = static_cast<PerlinNoiseParams*>(params);
                ImGui::InputInt("Octaves", &perlinParams->octaves);
            }
        };
    }
    void Draw() {
        if (ImGui::Button("Add Operator")) {
            m_showOperatorSelector = true;
        }

        // Popup de sélection d'opérateur
        if (m_showOperatorSelector) {
            ImGui::OpenPopup("Select Operator");
        }

        if (ImGui::BeginPopup("Select Operator")) {
            for (const auto& [name, info] : m_operators) {
                if (ImGui::Selectable(name.c_str())) {
                    m_selectedOperatorType = name;
                    m_currentParams = info.createDefaultParams();
                    m_showOperatorSelector = false;
                    m_isModalOpen = true;
                }
            }
            ImGui::EndPopup();
        }

        if(m_isModalOpen)
        {
            ImGui::OpenPopup("Configure Operator");
        }

        // Popup de configuration des paramètres
        if (ImGui::BeginPopupModal("Configure Operator", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            if (m_currentParams) {
                ImGui::Text("Configure %s", m_selectedOperatorType.c_str());
                ImGui::Separator();

                auto& info = m_operators[m_selectedOperatorType];
                info.drawParamsUI(m_currentParams.get());

                ImGui::Separator();

                if (ImGui::Button("Add to Stack")) {
                    auto opType = m_currentParams->GetType();
                    auto op = info.factory(m_currentParams);
                    if (op) {
                        switch(opType)
                        {
                            case OperatorType::PerlinType:
                                m_currentTexture->AddPerlinOperator(static_cast<PerlinNoiseParams*>(m_currentParams.get())->octaves);
                                break;
                            default:
                                break;
                        }
                    }
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("Cancel")) {
                    ImGui::CloseCurrentPopup();
                }
            }
            m_isModalOpen = false;
            ImGui::EndPopup();
        }

        // Afficher la pile d'opérateurs
        if (ImGui::BeginChild("OperatorStack", ImVec2(0, 200), true)) {
            if(m_currentTexture == nullptr) return;
            for (size_t i = 0; i < m_currentTexture->GetOperators().size(); i++) {
                ImGui::PushID(i);
                std::cout << m_currentTexture->GetOperators()[i]->GetOperatorName() << std::endl;
                ImGui::Text("%d. %s", i + 1, m_currentTexture->GetOperators()[i]->GetOperatorName().c_str());
                
                ImGui::SameLine();
                // if (ImGui::Button("X")) {
                //     m_operatorStack.erase(m_operatorStack.begin() + i);
                // }
                ImGui::PopID();
            }
        }
        ImGui::EndChild();
    }

private:
    std::unordered_map<std::string, OperatorInfo> m_operators;
    bool m_showOperatorSelector = false;
    bool m_isModalOpen = false;
    std::shared_ptr<OperatorParams> m_currentParams = nullptr;
    std::string m_selectedOperatorType;
    std::vector<std::unique_ptr<Operator>> m_operatorStack;
    std::shared_ptr<Texture> m_currentTexture;
};
