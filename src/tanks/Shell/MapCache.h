#pragma once
#include <map>
#include <memory>
#include <string>

class World;
namespace FileSystem
{
	class IFileSystem;
}

class MapCache
{
public:
	const World& GetCachedWorld(FileSystem::IFileSystem &fs, const std::string &mapName);
	std::unique_ptr<World> CheckoutCachedWorld(FileSystem::IFileSystem &fs, const std::string &mapName);

private:
	std::map<std::string, std::unique_ptr<World>> _cachedWorlds;
};
