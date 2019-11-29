#include "config/VariableTable.h"
#include "config/VariableArray.h"

// first pass to define the structure
#include "Configuration.h"

static void InitProfiles(VariableTable &profiles)
{
	ConfControllerProfile(&profiles.GetTable("WASD"));

	ConfControllerProfile arrows(&profiles.GetTable("Arrows"));
	arrows.key_forward.Set("Up");
	arrows.key_back.Set("Down");
	arrows.key_left.Set("Left");
	arrows.key_right.Set("Right");
	arrows.key_fire.Set("Space");
	arrows.key_light.Set("Delete");
	arrows.key_tower_left.Set("8");
	arrows.key_tower_center.Set("9");
	arrows.key_tower_right.Set("0");
	arrows.key_no_pickup.Set("Right Shift");
	arrows.lights.Set(true);
	arrows.aim_to_mouse.Set(false);
	arrows.move_to_mouse.Set(false);
	arrows.arcade_style.Set(false);
}

static void InitLobbyList(VariableArray &lobby_servers)
{
	lobby_servers.PushBack(VariableBase::typeString).AsStr().Set("tzod.fatal.ru/lobby");
}

// second time include it to implement initialize function
#define CONFIG_CACHE_PASS2
#include "Configuration.h"
#undef CONFIG_CACHE_PASS2

/////////////////////////////////////////////
// Utility access functions

#include "as/AppConfig.h"
#include <algorithm>

int GetCurrentTier(const ShellConfig &conf, const DMCampaign &dmCampaign)
{
	return std::max(0, std::min((int)dmCampaign.tiers.GetSize() - 1, conf.sp_tier.GetInt()));
}

int GetCurrentTierMapCount(const ShellConfig &conf, const DMCampaign &dmCampaign)
{
	DMCampaignTier tierDesc(&dmCampaign.tiers.GetTable(GetCurrentTier(conf, dmCampaign)));
	return static_cast<int>(tierDesc.maps.GetSize());
}

int GetCurrentMap(const ShellConfig &conf, const DMCampaign &dmCampaign)
{
	return std::max(0, std::min(GetCurrentTierMapCount(conf, dmCampaign) - 1, conf.sp_map.GetInt()));
}

