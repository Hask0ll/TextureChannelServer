#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include "Network/NetWorkClient.h"
#include "../Client/Texture/Texture.h"

// This class manages the sharing of textures between clients through the network
class TextureNetworkManager {
public:
    TextureNetworkManager(std::shared_ptr<NetworkClient> client);
    ~TextureNetworkManager() = default;

    // Connect to server
    bool Connect(const std::string& username);

    // Disconnects from server
    void Disconnect();

    // Check connection status
    bool IsConnected() const;

    // Share a texture with other clients
    void ShareTexture(std::shared_ptr<Texture> texture);

    // Request a texture from another client
    void RequestTexture(const std::string& sourceClient, const std::string& textureName);

    // Set callback for when a remote texture becomes available
    void SetRemoteTextureAvailableCallback(std::function<void(const std::string&, const std::string&)> callback);

    // Set callback for when a texture is received
    void SetTextureReceivedCallback(std::function<void(const std::string&, std::shared_ptr<Texture>)> callback);

    // Set callback for connected/disconnected clients
    void SetClientStatusCallback(std::function<void(const std::string&, bool)> callback);

    // Set callback for chat messages
    void SetChatMessageCallback(std::function<void(const std::string&, const std::string&)> callback);

    // Send a chat message
    void SendChatMessage(const std::string& message);

private:
    // Handle network message (called from NetworkClient callback)
    void HandleNetworkMessage(const std::string& message);

    // Handle received texture data (called from NetworkClient callback)
    void HandleTextureData(const std::string& textureName, const std::vector<unsigned char>& data);

    // Parse protocol messages
    void HandleWelcomeMessage(const std::string& message);
    void HandleConnectMessage(const std::string& message);
    void HandleDisconnectMessage(const std::string& message);
    void HandleRenameMessage(const std::string& message);
    void HandleChatMessage(const std::string& message);
    void HandleTextureAvailableMessage(const std::string& message);
    void HandleTextureListMessage(const std::string& message);

    // Create a new texture from raw data
    std::shared_ptr<Texture> CreateTextureFromData(const std::string& name, const std::vector<unsigned char>& data);

    std::shared_ptr<NetworkClient> m_client;
    std::string m_username;

    // Callbacks
    std::function<void(const std::string&, const std::string&)> m_remoteTextureAvailableCallback;
    std::function<void(const std::string&, std::shared_ptr<Texture>)> m_textureReceivedCallback;
    std::function<void(const std::string&, bool)> m_clientStatusCallback;
    std::function<void(const std::string&, const std::string&)> m_chatMessageCallback;

    // Map of available remote textures (client -> vector of texture names)
    std::map<std::string, std::vector<std::string>> m_availableRemoteTextures;
};