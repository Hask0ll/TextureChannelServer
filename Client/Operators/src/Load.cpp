#include "Operators/include/Load.h"
#include "Application/Application.h"


void LoadOperator::process(std::string& name)
{
    m_data = Application::Get().GetStorage()->Load(name);
}
