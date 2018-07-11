#include "Raven_Scriptor.h"
#include "Config.h"

Raven_Scriptor* Raven_Scriptor::Instance()
{
  static Raven_Scriptor instance;

  return &instance;
}


Raven_Scriptor::Raven_Scriptor()
	:Scriptor()
{
  RunScriptFile("E:\\BattleTanks\\tanks\\data\\scripts\\Params.lua");
}