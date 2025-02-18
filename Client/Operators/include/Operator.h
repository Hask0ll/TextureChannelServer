#pragma once
#include <string>

class Operator
{
public:
    virtual std::string GetOperatorName() = 0;
    virtual void Draw() = 0;
    virtual std::vector<unsigned char> GetData() = 0;

	template <typename T>
    T Serialize() {
		return static_cast<T>(*this);
    }
};
