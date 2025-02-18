#pragma once
#include <list>
#include <vector>
#include "Operator.h"

class LoadOperator: public Operator
{
public:
    virtual std::string GetOperatorName() override
    {
        return "Load";
    }

    LoadOperator() = default;

    void process(std::string& name, std::list<std::vector<unsigned char>> stack);
  
    virtual size_t GetSerializedSize() const override
    {
        return 0;
    }
    virtual void Serialize(char* buffer) const override
    {
        
    }
    virtual void Deserialize(const char* buffer) override
    {
        
    }

    virtual void Draw() override
    {
        // do nothing
    }
private:
    std::vector<unsigned char> m_data;
};
