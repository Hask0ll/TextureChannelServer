#include "Operators/include/Save.h"

#include "Application/Application.h"


void SaveOperator::process(std::string& name, std::vector<unsigned char>& data)
{
    m_data = data;
    Application::Get().GetStorage()->Save(name, m_data);
}
