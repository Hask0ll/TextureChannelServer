#include "Operators/include/Save.h"

#include "Application/Application.h"


void SaveOperator::process(std::string& name, std::list<std::vector<unsigned char>> stack)
{
    auto data = stack.back();
    Application::Get().GetStorage()->Save(name, data);
}
