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
    SaveType,
    LoadType,
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

struct ColorizerParams : public OperatorParams {
    ImVec4 tintColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    OperatorType GetType() override { return OperatorType::ColorizerType; }
};

struct LoadParams : public OperatorParams {
    OperatorType GetType() override { return OperatorType::LoadType; }
    char name[256];
};

struct SaveParams : public OperatorParams {
    OperatorType GetType() override { return OperatorType::SaveType; }
    char name[256];
};

class OperatorManager
{
    struct OperatorInfo {
        std::string name;
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
            []() {
                return std::make_shared<PerlinNoiseParams>();
            },
            [](OperatorParams* params) {
                auto perlinParams = static_cast<PerlinNoiseParams*>(params);
                ImGui::InputInt("Octaves", &perlinParams->octaves);
            }
        };

        m_operators["Colorizer"] = {
            "Colorizer",
            []() {
                return std::make_shared<ColorizerParams>();
            },
            [](OperatorParams* params) {
                auto colorizerParams = static_cast<ColorizerParams*>(params);
                ImGui::ColorEdit4("Tint Color", (float*)&colorizerParams->tintColor);
            }
        };

        m_operators["Save"] = {
            "Save",
            []() {
                return std::make_shared<SaveParams>();
            },
            [](OperatorParams* params) {
                auto saveParams = static_cast<SaveParams*>(params);
                ImGui::InputText("##texturename", saveParams->name, sizeof(saveParams->name));
            }
        };

        m_operators["Load"] = {
            "Load",
            []() {
                return std::make_shared<LoadParams>();
            },
            [](OperatorParams* params) {
                auto loadParams = static_cast<LoadParams*>(params);
                ImGui::InputText("##texturename", loadParams->name, sizeof(loadParams->name));
            }
        };
    }
    void Draw() {
        if (ImGui::Button("Add Operator")) {
            m_showOperatorSelector = true;
        }

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

        if (ImGui::BeginPopupModal("Configure Operator", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            if (m_currentParams) {
                ImGui::Text("Configure %s", m_selectedOperatorType.c_str());
                ImGui::Separator();

                auto& info = m_operators[m_selectedOperatorType];
                info.drawParamsUI(m_currentParams.get());

                ImGui::Separator();

                if (ImGui::Button("Add to Stack")) {
                    auto opType = m_currentParams->GetType();
                    switch(opType)
                    {
                        case OperatorType::PerlinType:
                            {
                                m_currentTexture->AddPerlinOperator(static_cast<PerlinNoiseParams*>(m_currentParams.get())->octaves);
                                break;
                            }
                        case OperatorType::ColorizerType:
                            {
                                auto colorizerParams = static_cast<ColorizerParams*>(m_currentParams.get());
                                if(m_currentTexture == nullptr) return;
                                auto color = glm::vec3(colorizerParams->tintColor.x, colorizerParams->tintColor.y, colorizerParams->tintColor.z);
                                m_currentTexture->AddColorizerOperator(color);
                                break;
                            }
                        case OperatorType::LoadType:
                            {
                                auto laodParams = static_cast<LoadParams*>(m_currentParams.get());
                                m_currentTexture->AddLoadOperator(std::string(laodParams->name));
                                break;
                            }
                        case OperatorType::SaveType:
                            {
                                auto saveParams = static_cast<SaveParams*>(m_currentParams.get());
                                m_currentTexture->AddSaveOperator(std::string(saveParams->name));
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

        // Display the stack of operators
        if (ImGui::BeginChild("OperatorStack", ImVec2(0, 200), true)) {
            if(m_currentTexture == nullptr) return;
            for (size_t i = 0; i < m_currentTexture->GetOperators().size(); i++) {
                ImGui::PushID(i);
                std::cout << m_currentTexture->GetOperators()[i]->GetOperatorName() << std::endl;
                ImGui::Text("%d. %s", i + 1, m_currentTexture->GetOperators()[i]->GetOperatorName().c_str());
                
                ImGui::SameLine();
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
