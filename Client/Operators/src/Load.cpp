#include "Operators/include/Load.h"
#include "Application/Application.h"



void LoadOperator::process(std::string& name, std::list<std::vector<unsigned char>> stack)
{
    auto data = Application::Get().GetStorage()->Load(name);
    stack.push_back(data);
}
