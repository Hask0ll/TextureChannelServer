#include "NetworkUI.h"

NetworkUI::NetworkUI(std::shared_ptr<TextureNetworkManager> networkManager)
    : m_networkManager(networkManager)
    , m_showConnectionWindow(false)
    , m_isConnected(false)
{
    // Initialize UI state
    strcpy_s(m_serverAddress, "127.0.0.1");
    strcpy_s(m_serverPort, "13579");
    strcpy_s(m_username, "User");
    memset(m_chatMessage, 0, sizeof(m_chatMessage));

    // Set up callbacks
    m_networkManager->SetRemoteTextureAvailableCallback([this](const std::string& clientName, const std::string& textureName) {
        this->OnRemoteTextureAvailable(clientName, textureName);
        });

    m_networkManager->SetTextureReceivedCallback([this](const std::string& textureName, std::shared_ptr<Texture> texture) {
        this->OnTextureReceived(textureName, texture);
        });

    m_networkManager->SetClientStatusCallback([this](const std::string& clientName, bool connected) {
        this->OnClientStatusChanged(clientName, connected);
        });

    m_networkManager->SetChatMessageCallback([this](const std::string& sender, const std::string& message) {
        this->OnChatMessageReceived(sender, message);
        });
}

void NetworkUI::Draw() {
    // Main menu bar
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Network")) {
            if (ImGui::MenuItem("Connect", nullptr, &m_showConnectionWindow)) {
                m_showConnectionWindow = true;
            }

            if (ImGui::MenuItem("Disconnect", nullptr, false, m_isConnected)) {
                m_networkManager->Disconnect();
                m_isConnected = false;

                // Add system message to chat
                m_chatHistory.push_back({ "System", "Disconnected from server", true });
            }

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    // Connection window
    if (m_showConnectionWindow) {
        DrawConnectionPanel();
    }

    // Main network UI (only when connected)
    if (m_isConnected) {
        // Create a dockspace for the network UI
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove;

        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 300, 20), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(300, ImGui::GetIO().DisplaySize.y - 20), ImGuiCond_FirstUseEver);

        if (ImGui::Begin("Network", nullptr, window_flags)) {
            // Draw the clients list
            DrawClientList();

            // Draw the texture sharing panel
            DrawTextureSharingPanel();

            // Draw the chat panel
            DrawChatPanel();
        }
        ImGui::End();
    }
}

void NetworkUI::DrawConnectionPanel() {
    ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Connect to Server", &m_showConnectionWindow)) {
        ImGui::Text("Server Connection");
        ImGui::Separator();

        ImGui::InputText("Server Address", m_serverAddress, sizeof(m_serverAddress));
        ImGui::InputText("Port", m_serverPort, sizeof(m_serverPort));
        ImGui::InputText("Username", m_username, sizeof(m_username));

        if (ImGui::Button("Connect")) {
            // Try to connect to the server
            m_networkManager->Disconnect(); // Disconnect if already connected

            if (m_networkManager->Connect(m_username)) {
                m_isConnected = true;
                m_showConnectionWindow = false;

                // Add system message to chat
                m_chatHistory.push_back({ "System", "Connected to server", true });

            }
            else {
                // Failed to connect
                m_chatHistory.push_back({ "System", "Failed to connect to server", true });
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel")) {
            m_showConnectionWindow = false;
        }
    }
    ImGui::End();
}

void NetworkUI::DrawClientList() {
    if (ImGui::CollapsingHeader("Online Clients", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::BeginChild("ClientList", ImVec2(0, 120), true)) {
            for (const auto& [clientName, client] : m_remoteClients) {
                if (client.connected) {
                    ImGui::Text("%s", clientName.c_str());

                    // Show available textures when client is clicked
                    if (ImGui::IsItemClicked() && ImGui::BeginPopupContextItem(clientName.c_str())) {
                        if (client.availableTextures.empty()) {
                            ImGui::Text("No textures available");
                        }
                        else {
                            ImGui::Text("Available Textures:");
                            for (const auto& textureName : client.availableTextures) {
                                if (ImGui::MenuItem(textureName.c_str())) {
                                    // Request the texture
                                    m_networkManager->RequestTexture(clientName, textureName);

                                    // Add system message to chat
                                    m_chatHistory.push_back({ "System", "Requested texture '" + textureName + "' from " + clientName, true });
                                }
                            }
                        }
                        ImGui::EndPopup();
                    }
                }
            }
        }
        ImGui::EndChild();
    }
}

void NetworkUI::DrawChatPanel() {
    if (ImGui::CollapsingHeader("Chat", ImGuiTreeNodeFlags_DefaultOpen)) {
        // Chat history
        if (ImGui::BeginChild("ChatHistory", ImVec2(0, 200), true)) {
            for (const auto& message : m_chatHistory) {
                if (message.isSystem) {
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "[%s] %s",
                        message.sender.c_str(), message.content.c_str());
                }
                else {
                    ImGui::Text("[%s] %s", message.sender.c_str(), message.content.c_str());
                }
            }

            // Auto-scroll to bottom
            if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                ImGui::SetScrollHereY(1.0f);
        }
        ImGui::EndChild();

        // Chat input
        ImGui::PushItemWidth(-1);
        if (ImGui::InputText("##ChatInput", m_chatMessage, sizeof(m_chatMessage), ImGuiInputTextFlags_EnterReturnsTrue)) {
            if (strlen(m_chatMessage) > 0) {
                // Send chat message
                m_networkManager->SendChatMessage(m_chatMessage);

                // Add message to local chat history
                m_chatHistory.push_back({ "Me", m_chatMessage, false });

                // Clear input
                memset(m_chatMessage, 0, sizeof(m_chatMessage));
            }
        }
        ImGui::SetItemDefaultFocus();
    }
}

void NetworkUI::DrawTextureSharingPanel() {
    if (ImGui::CollapsingHeader("Texture Sharing", ImGuiTreeNodeFlags_DefaultOpen)) {
        // Current texture info
        if (m_currentTexture) {
            ImGui::Text("Current Texture: %s", m_currentTexture->GetName().c_str());

            if (ImGui::Button("Share Texture")) {
                m_networkManager->ShareTexture(m_currentTexture);

                // Add system message to chat
                m_chatHistory.push_back({ "System", "Shared texture '" + m_currentTexture->GetName() + "'", true });
            }
        }
        else {
            ImGui::Text("No texture selected");
        }

        // Received textures
        if (!m_receivedTextures.empty()) {
            ImGui::Separator();
            ImGui::Text("Received Textures:");

            if (ImGui::BeginChild("ReceivedTextures", ImVec2(0, 120), true)) {
                for (const auto& texture : m_receivedTextures) {
                    ImGui::Text("%s", texture->GetName().c_str());
                }
            }
            ImGui::EndChild();
        }
    }
}

void NetworkUI::SetCurrentTexture(std::shared_ptr<Texture> texture) {
    m_currentTexture = texture;
}

void NetworkUI::AddReceivedTexture(std::shared_ptr<Texture> texture) {
    // Check if we already have this texture
    for (const auto& existing : m_receivedTextures) {
        if (existing->GetName() == texture->GetName()) {
            return;
        }
    }

    m_receivedTextures.push_back(texture);
}

std::shared_ptr<Texture> NetworkUI::GetReceivedTexture(const std::string& name) {
    for (const auto& texture : m_receivedTextures) {
        if (texture->GetName() == name) {
            return texture;
        }
    }
    return nullptr;
}

const std::vector<std::shared_ptr<Texture>>& NetworkUI::GetReceivedTextures() const {
    return m_receivedTextures;
}

void NetworkUI::OnRemoteTextureAvailable(const std::string& clientName, const std::string& textureName) {
    // Add texture to client's available textures
    if (m_remoteClients.find(clientName) == m_remoteClients.end()) {
        RemoteClient client;
        client.name = clientName;
        client.connected = true;
        m_remoteClients[clientName] = client;
    }

    // Add texture to client's list if not already present
    auto& availableTextures = m_remoteClients[clientName].availableTextures;
    if (std::find(availableTextures.begin(), availableTextures.end(), textureName) == availableTextures.end()) {
        availableTextures.push_back(textureName);
    }

    // Add system message to chat
    m_chatHistory.push_back({ "System", "New texture '" + textureName + "' available from " + clientName, true });
}

void NetworkUI::OnTextureReceived(const std::string& textureName, std::shared_ptr<Texture> texture) {
    // Add the texture to received textures
    AddReceivedTexture(texture);

    // Add system message to chat
    m_chatHistory.push_back({ "System", "Received texture '" + textureName + "'", true });
}

void NetworkUI::OnClientStatusChanged(const std::string& clientName, bool connected) {
    if (m_remoteClients.find(clientName) == m_remoteClients.end()) {
        RemoteClient client;
        client.name = clientName;
        client.connected = connected;
        m_remoteClients[clientName] = client;
    }
    else {
        m_remoteClients[clientName].connected = connected;
    }

    // Add system message to chat
    if (connected) {
        m_chatHistory.push_back({ "System", clientName + " connected", true });
    }
    else {
        m_chatHistory.push_back({ "System", clientName + " disconnected", true });
    }
}

void NetworkUI::OnChatMessageReceived(const std::string& sender, const std::string& message) {
    m_chatHistory.push_back({ sender, message, false });
}