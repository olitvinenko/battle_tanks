// TextureManager.h

#pragma once

#include "RenderBase.h"

#include <list>
#include <map>
#include <memory>
#include <vector>
#include "FileSystem.h"

struct LogicalTexture
{
	Texture dev_texture;

	Vector2 uvPivot;

	float pxFrameWidth;
	float pxFrameHeight;
	float pxBorderSize;

	std::vector<math::RectFloat> uvFrames;
};

class TextureManager
{
public:
	TextureManager(TextureManager&&) = default;
	TextureManager(IRender &render);
	~TextureManager();

	IRender& GetRender() const { return _render; }

	int LoadPackage(const std::string &packageName, std::shared_ptr<FileSystem::File::Memory> file, FileSystem::IFileSystem &fs);
	int LoadDirectory(const std::string &dirName, const std::string &texPrefix, FileSystem::IFileSystem &fs);
	void UnloadAllTextures();

	size_t FindSprite(const std::string &name) const;
	const LogicalTexture& GetSpriteInfo(size_t texIndex) const { return _logicalTextures[texIndex]; }
	float GetFrameWidth(size_t texIndex, size_t /*frameIdx*/) const { return _logicalTextures[texIndex].pxFrameWidth; }
	float GetFrameHeight(size_t texIndex, size_t /*frameIdx*/) const { return _logicalTextures[texIndex].pxFrameHeight; }
	float GetBorderSize(size_t texIndex) const { return _logicalTextures[texIndex].pxBorderSize; }
	unsigned int GetFrameCount(size_t texIndex) const { return static_cast<unsigned int>(_logicalTextures[texIndex].uvFrames.size()); }

	void GetTextureNames(std::vector<std::string> &names, const char *prefix, bool noPrefixReturn) const;

	float GetCharHeight(size_t fontTexture) const;

protected:
	IRender &_render;

	struct TexDesc
	{
		Texture id;
		int width;          // The Width Of The Entire Image.
		int height;         // The Height Of The Entire Image.
		int refCount;       // number of logical textures
	};
	typedef std::list<TexDesc>       TexDescList;
	typedef TexDescList::iterator    TexDescIterator;

	typedef std::map<std::string, TexDescIterator> FileToTexDescMap;
	typedef std::map<Texture, TexDescIterator> DevToTexDescMap;

	FileToTexDescMap _mapFile_to_TexDescIter;
	DevToTexDescMap  _mapDevTex_to_TexDescIter;
	TexDescList      _textures;
	std::map<std::string, size_t>   _mapName_to_Index;// index in _logicalTextures
	std::vector<LogicalTexture>  _logicalTextures;

	void LoadTexture(TexDescIterator &itTexDesc, const std::string &fileName, FileSystem::IFileSystem &fs);
	void Unload(TexDescIterator what);

	void CreateChecker(); // Create checker texture without name and with index=0
};

// end of file
