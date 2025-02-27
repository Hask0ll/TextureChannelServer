#pragma once
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

class Storage
{
public:
    Storage() = default;

    std::vector<unsigned char> Load(std::string& name)
    {
        if(m_storage.find(name) != m_storage.end())
        {
            return m_storage[name];
        }
        throw std::runtime_error("No such key in storage");
    }

    void Save(std::string& name, std::vector<unsigned char>& data)
    {
        if(m_storage.find(name) != m_storage.end())
        {
            throw std::runtime_error("Key already exists in storage");
        }
        m_storage[name] = data;
    }
    
private:
    std::map<std::string, std::vector<unsigned char>> m_storage;
};
