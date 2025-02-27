#include "TextureNetworkManager.h"
#include <sstream>
#include <algorithm>

TextureNetworkManager::TextureNetworkManager(std::shared_ptr<NetworkClient> client)
    : m_client(client)
{
    // Set up callbacks
    m_client->SetMessageCallback([this](const std::string& message) {
        this->HandleNetworkMessage(message);
        });

    m_client->SetTextureReceivedCallback([this](const std::string& textureName, const std::vector<unsigned char>& data) {
        this->HandleTextureData(textureName, data);
        });
}

bool TextureNetworkManager::Connect(const std::string& username) {
    try {
        m_username = username;
        m_client->SetClientName(username);
        m_client->Connect();
        m_client->StartListening();
        return true;
    }
    catch (const std::exception& e) {
        return false;
    }
}

void TextureNetworkManager::Disconnect() {
    m_client->Disconnect();
}

bool TextureNetworkManager::IsConnected() const {
    return m_client->IsConnected();
}

void TextureNetworkManager::ShareTexture(std::shared_ptr<Texture> texture) {
    if (!IsConnected()) {
        return;
    }

    try {
        // Get the texture data
        if (texture->stack.empty()) {
            return;
        }

        // Send the texture data to the server
        const auto& data = texture->stack.back();
        m_client->SendTextureData(texture->GetName(), data);

    }
    catch (const std::exception& e) {
    }
}

void TextureNetworkManager::RequestTexture(const std::string& sourceClient, const std::string& textureName) {
    if (!IsConnected()) {
        return;
    }

    try {
        m_client->RequestTexture(sourceClient, textureName);
    }
    catch (const std::exception& e) {
    }
}

void TextureNetworkManager::SetRemoteTextureAvailableCallback(std::function<void(const std::string&, const std::string&)> callback) {
    m_remoteTextureAvailableCallback = std::move(callback);
}

void TextureNetworkManager::SetTextureReceivedCallback(std::function<void(const std::string&, std::shared_ptr<Texture>)> callback) {
    m_textureReceivedCallback = std::move(callback);
}

void TextureNetworkManager::SetClientStatusCallback(std::function<void(const std::string&, bool)> callback) {
    m_clientStatusCallback = std::move(callback);
}

void TextureNetworkManager::SetChatMessageCallback(std::function<void(const std::string&, const std::string&)> callback) {
    m_chatMessageCallback = std::move(callback);
}

void TextureNetworkManager::SendChatMessage(const std::string& message) {
    if (!IsConnected()) {
        return;
    }

    try {
        m_client->SendTextMessage(message);
    }
    catch (const std::exception& e) {
    }
}

void TextureNetworkManager::HandleNetworkMessage(const std::string& message) {
    // Protocol: messages are in the format TYPE:DATA
    size_t separatorPos = message.find(':');
    if (separatorPos == std::string::npos) {
        return;
    }

    std::string type = message.substr(0, separatorPos);
    std::string data = message.substr(separatorPos + 1);

    if (type == "WELCOME") {
        HandleWelcomeMessage(data);
    }
    else if (type == "CONNECT") {
        HandleConnectMessage(data);
    }
    else if (type == "DISCONNECT") {
        HandleDisconnectMessage(data);
    }
    else if (type == "RENAME") {
        HandleRenameMessage(data);
    }
    else if (type == "CHAT") {
        HandleChatMessage(data);
    }
    else if (type == "TEXTURE_AVAILABLE") {
        HandleTextureAvailableMessage(data);
    }
    else if (type == "TEXTURE_LIST") {
        HandleTextureListMessage(data);
    }
    else {
    }
}

void TextureNetworkManager::HandleTextureData(const std::string& textureName, const std::vector<unsigned char>& data) {

    // Create a new texture from the received data
    auto texture = CreateTextureFromData(textureName, data);

    // Notify about received texture
    if (m_textureReceivedCallback) {
        m_textureReceivedCallback(textureName, texture);
    }
}

void TextureNetworkManager::HandleWelcomeMessage(const std::string& message) {

    // Update username if the server assigned a different one
    if (message != m_username) {
        m_username = message;
    }
}

void TextureNetworkManager::HandleConnectMessage(const std::string& message) {

    // Notify about new client
    if (m_clientStatusCallback) {
        m_clientStatusCallback(message, true);
    }
}

void TextureNetworkManager::HandleDisconnectMessage(const std::string& message) {

    // Remove client's textures from available list
    m_availableRemoteTextures.erase(message);

    // Notify about disconnected client
    if (m_clientStatusCallback) {
        m_clientStatusCallback(message, false);
    }
}

void TextureNetworkManager::HandleRenameMessage(const std::string& message) {
    // Format: oldName:newName
    size_t separatorPos = message.find(':');
    if (separatorPos == std::string::npos) {
        return;
    }

    std::string oldName = message.substr(0, separatorPos);
    std::string newName = message.substr(separatorPos + 1);


    // Update available textures map if needed
    if (m_availableRemoteTextures.find(oldName) != m_availableRemoteTextures.end()) {
        m_availableRemoteTextures[newName] = m_availableRemoteTextures[oldName];
        m_availableRemoteTextures.erase(oldName);
    }
}

void TextureNetworkManager::HandleChatMessage(const std::string& message) {
    // Format: sender:content
    size_t separatorPos = message.find(':');
    if (separatorPos == std::string::npos) {
        return;
    }

    std::string sender = message.substr(0, separatorPos);
    std::string content = message.substr(separatorPos + 1);


    // Notify about chat message
    if (m_chatMessageCallback) {
        m_chatMessageCallback(sender, content);
    }
}

void TextureNetworkManager::HandleTextureAvailableMessage(const std::string& message) {
    // Format: clientName:textureName
    size_t separatorPos = message.find(':');
    if (separatorPos == std::string::npos) {
        return;
    }

    std::string clientName = message.substr(0, separatorPos);
    std::string textureName = message.substr(separatorPos + 1);


    // Add to available textures
    if (m_availableRemoteTextures.find(clientName) == m_availableRemoteTextures.end()) {
        m_availableRemoteTextures[clientName] = std::vector<std::string>();
    }

    m_availableRemoteTextures[clientName].push_back(textureName);

    // Notify about new texture
    if (m_remoteTextureAvailableCallback) {
        m_remoteTextureAvailableCallback(clientName, textureName);
    }
}

void TextureNetworkManager::HandleTextureListMessage(const std::string& message) {
    // Format: clientName1:textureName1;clientName2:textureName2;...
    std::stringstream ss(message);
    std::string item;

    while (std::getline(ss, item, ';')) {
        size_t separatorPos = item.find(':');
        if (separatorPos == std::string::npos) {
            continue;
        }

        std::string clientName = item.substr(0, separatorPos);
        std::string textureName = item.substr(separatorPos + 1);

        // Add to available textures
        if (m_availableRemoteTextures.find(clientName) == m_availableRemoteTextures.end()) {
            m_availableRemoteTextures[clientName] = std::vector<std::string>();
        }

        m_availableRemoteTextures[clientName].push_back(textureName);

        // Notify about available texture
        if (m_remoteTextureAvailableCallback) {
            m_remoteTextureAvailableCallback(clientName, textureName);
        }
    }
}

std::shared_ptr<Texture> TextureNetworkManager::CreateTextureFromData(const std::string& name, const std::vector<unsigned char>& data) {
    // Create a new texture with the given name
    std::string textureName = name;
    auto texture = std::make_shared<Texture>(textureName);

    // Add the data to the texture stack
    texture->stack.push_back(data);

    return texture;
}