#pragma once
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

    void process(std::string& name, std::vector<unsigned char>& data);

    virtual void Draw() override
    {
        // do nothing
    }
private:
    std::vector<unsigned char> m_data;
};
