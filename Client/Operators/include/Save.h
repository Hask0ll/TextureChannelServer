#pragma once
#include <list>
#include <vector>

#include "Operator.h"

class SaveOperator: public Operator 
{
public:

    virtual std::string GetOperatorName() override
    {
        return "Save";
    }

    SaveOperator() = default;

    std::vector<unsigned char> GetData()
    {
        return m_data;
    }
    void process(std::string& name, std::list<std::vector<unsigned char>> stack);

    virtual void Draw() override
    {
        // do nothing
    }

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
private:
    std::vector<unsigned char> m_data;
};
