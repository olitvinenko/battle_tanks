#pragma once
#include <memory>
#include <string>

class VariableString;
namespace UI
{
	template<class T> struct DataSource;
}

std::shared_ptr<UI::DataSource<const std::string&>> ConfBind(VariableString& confString);
