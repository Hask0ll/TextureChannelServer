#pragma once
#include <string>

class Operator
{
public:
    virtual std::string GetOperatorName() = 0;
    virtual void Draw(std::list<std::vector<unsigned char>> stack) = 0;
	virtual size_t GetSerializedSize() const = 0;
	virtual void Serialize(char* buffer) const = 0;
	virtual void Deserialize(const char* buffer) = 0;
};
