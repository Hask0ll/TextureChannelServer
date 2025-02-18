#pragma once
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

    void process(std::string& name);

    std::vector<unsigned char> GetData()
    {
        return m_data;
    }

    virtual void Draw() override
    {
        // do nothing
    }
private:
    std::vector<unsigned char> m_data;
};
