#pragma once
#include <string>

class Operator
{
public:
    virtual std::string GetOperatorName() = 0;
    virtual void Draw() = 0;
    virtual std::vector<unsigned char> GetData() = 0;
};
