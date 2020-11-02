#include "Controller.h"
#include "InputManager.h"
#include "Configuration.h"
#include "ui/ConsoleBuffer.h"

InputManager::InputManager(ShellConfig &conf, UI::ConsoleBuffer &logger)
	: _conf(conf)
	, _logger(logger)
{
	_conf.dm_profiles.eventChange = std::bind(&InputManager::OnProfilesChange, this);
	OnProfilesChange();
}

InputManager::~InputManager()
{
	_conf.dm_profiles.eventChange = nullptr;
}

Controller* InputManager::GetController(unsigned int index)
{
	return index < _controllers.size() ? &_controllers[index] : nullptr;
}

const Controller* InputManager::GetController(unsigned int index) const
{
	return index < _controllers.size() ? &_controllers[index] : nullptr;
}

void InputManager::Step(float dt)
{
    for (auto &controller: _controllers)
    {
        controller.Step(dt);
    }
}

void InputManager::OnProfilesChange()
{
	_controllers.clear();

	for (auto &key: _conf.dm_profiles.GetKeys())
	{
		_controllers.emplace_back();

		VariableBase *p = _conf.dm_profiles.Find(key.c_str());
		if (p && VariableBase::TABLE == p->GetType())
		{
			ConfControllerProfile t(&p->AsTable());
			_controllers.back().SetProfile(t);
		}
		else
		{
			_logger.Printf(1, "controller profile '%s' not found", key.c_str());
		}
	}
}
