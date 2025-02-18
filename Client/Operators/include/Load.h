#pragma once
#include "Operator.h"

class Load: public Operator
{
public:
    virtual std::string GetOperatorName() override
    {
        return "Load";
    }

    Load() = default;

    void process(std::string name)
    {
        
    }
};
