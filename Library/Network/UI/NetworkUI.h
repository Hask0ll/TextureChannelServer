#pragma once
#include <imgui.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include "../Library/Network/Texture/TextureNetworkManager.h"
#include "../Client/Texture/Texture.h"

// Structure to represent a remote client
struct RemoteClient {
    std::string name;
    bool connected;
    std::vector<std::string> availableTextures;
};

// UI class for the network features
class NetworkUI {
public:
    NetworkUI(std::shared_ptr<TextureNetworkManager> networkManager);
    ~NetworkUI() = default;

    // Draw the network UI
    void Draw();

    // Set the current texture to be shared
    void SetCurrentTexture(std::shared_ptr<Texture> texture);

    // Add a received texture
    void AddReceivedTexture(std::shared_ptr<Texture> texture);

    // Get a received texture by name
    std::shared_ptr<Texture> GetReceivedTexture(const std::string& name);

    // Get all received textures
    const std::vector<std::shared_ptr<Texture>>& GetReceivedTextures() const;

private:
    // Draw the connection panel
    void DrawConnectionPanel();

    // Draw the client list
    void DrawClientList();

    // Draw the chat panel
    void DrawChatPanel();

    // Draw the texture sharing panel
    void DrawTextureSharingPanel();

    // Callbacks for network events
    void OnRemoteTextureAvailable(const std::string& clientName, const std::string& textureName);
    void OnTextureReceived(const std::string& textureName, std::shared_ptr<Texture> texture);
    void OnClientStatusChanged(const std::string& clientName, bool connected);
    void OnChatMessageReceived(const std::string& sender, const std::string& message);

    std::shared_ptr<TextureNetworkManager> m_networkManager;
    std::shared_ptr<Texture> m_currentTexture;

    // UI state
    bool m_showConnectionWindow;
    bool m_isConnected;
    char m_serverAddress[256];
    char m_serverPort[16];
    char m_username[64];
    char m_chatMessage[512];

    // Chat history
    struct ChatMessage {
        std::string sender;
        std::string content;
        bool isSystem;
    };
    std::vector<ChatMessage> m_chatHistory;

    // Remote clients and their available textures
    std::map<std::string, RemoteClient> m_remoteClients;

    // Received textures
    std::vector<std::shared_ptr<Texture>> m_receivedTextures;
};