#pragma once
#include <Console.h>
#include <string>

class ShellConfig;

class ConfigConsoleHistory : public UI::IConsoleHistory
{
public:
	explicit ConfigConsoleHistory(const ShellConfig &conf);

	// UI::IConsoleHistory
	virtual void Enter(std::string str);
	virtual size_t GetItemCount() const;
	virtual const std::string& GetItem(size_t index) const;

private:
	const ShellConfig &_conf;
};
