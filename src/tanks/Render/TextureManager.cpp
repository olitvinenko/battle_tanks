#include "TextureManager.h"
#include "RenderBase.h"

#define TRACE(...)

#include "FileSystem.h"
#include "TGAImage.h"

extern "C"
{
	#include <lua.h>
	#include <lauxlib.h>
}

#include <cstring>

///////////////////////////////////////////////////////////////////////////////

class CheckerImage : public Image
{
public:
	// Image methods
	const void* GetData() const override { return _bytes; }
	unsigned int GetBitsPerPixel() const override { return 24; }
	unsigned int GetWidth() const override { return 4; }
	unsigned int GetHeight() const override { return 4; }

private:
	static const unsigned char _bytes[];
};

const unsigned char CheckerImage::_bytes[] = {
	0,  0,  0,     0,  0,  0,   255,255,255,   255,255,255,
	0,  0,  0,     0,  0,  0,   255,255,255,   255,255,255,
	255,255,255,   255,255,255,     0,  0,  0,     0,  0,  0,
	255,255,255,   255,255,255,     0,  0,  0,     0,  0,  0,
};


///////////////////////////////////////////////////////////////////////////////

TextureManager::TextureManager(IRender &render)
	: _render(render)
{
	CreateChecker();
}

TextureManager::~TextureManager()
{
	UnloadAllTextures();
}

void TextureManager::UnloadAllTextures()
{
	TexDescIterator it = _textures.begin();
	while (it != _textures.end())
		_render.TexFree((it++)->id);
	_textures.clear();
	_mapFile_to_TexDescIter.clear();
	_mapDevTex_to_TexDescIter.clear();
	_mapName_to_Index.clear();
	_logicalTextures.clear();
}

void TextureManager::LoadTexture(TexDescIterator &itTexDesc, const std::string &fileName, FileSystem::IFileSystem &fs)
{
	FileToTexDescMap::iterator it = _mapFile_to_TexDescIter.find(fileName);
	if (_mapFile_to_TexDescIter.end() != it)
	{
		itTexDesc = it->second;
	}
	else
	{
		std::shared_ptr<FileSystem::File::Memory> file = fs.Open(fileName)->AsMemory();
		std::unique_ptr<TgaImage> image(new TgaImage(file));

		TexDesc td;
		if (!_render.TexCreate(td.id, *image))
		{
			throw std::runtime_error("error in render device");
		}

		td.width = image->GetWidth();
		td.height = image->GetHeight();
		td.refCount = 0;

		_textures.push_front(td);
		itTexDesc = _textures.begin();
		_mapFile_to_TexDescIter[fileName] = itTexDesc;
		_mapDevTex_to_TexDescIter[itTexDesc->id] = itTexDesc;
	}
}

void TextureManager::Unload(TexDescIterator what)
{
	_render.TexFree(what->id);

	FileToTexDescMap::iterator it = _mapFile_to_TexDescIter.begin();
	while (_mapFile_to_TexDescIter.end() != it)
	{
		if (it->second->id == what->id)
		{
			_mapFile_to_TexDescIter.erase(it);
			break;
		}
		++it;
	}

	_mapDevTex_to_TexDescIter.erase(what->id);
	_textures.erase(what);
}

void TextureManager::CreateChecker()
{
	TexDesc td;
	LogicalTexture tex;


	//
	// check if checker texture already exists
	//

	assert(_logicalTextures.empty()); // to be sure that checker will get index 0
	assert(_mapName_to_Index.empty());

	TRACE("Creating checker texture...");



	//
	// create device texture
	//

	CheckerImage c;
	if (!_render.TexCreate(td.id, c))
	{
		TRACE("ERROR: error in render device");
		assert(false);
		return;
	}
	td.width = c.GetWidth();
	td.height = c.GetHeight();
	td.refCount = 0;

	_textures.push_front(td);
	TexDescIterator it = _textures.begin();



	//
	// create logical texture
	//

	tex.dev_texture = it->id;
	tex.uvPivot = Vector2(0, 0);
	tex.pxFrameWidth = (float)td.width * 8;
	tex.pxFrameHeight = (float)td.height * 8;
	tex.pxBorderSize = 0;

	math::RectFloat whole = { 0,0,8,8 };
	tex.uvFrames.push_back(whole);
	//---------------------
	_logicalTextures.push_back(tex);
	it->refCount++;
}

static int auxgetint(lua_State *L, int tblidx, const char *field, int def)
{
	lua_getfield(L, tblidx, field);
	if (lua_isnumber(L, -1)) def = lua_tointeger(L, -1);
	lua_pop(L, 1); // pop result of getfield
	return def;
}

static float auxgetfloat(lua_State *L, int tblidx, const char *field, float def)
{
	lua_getfield(L, tblidx, field);
	if (lua_isnumber(L, -1)) def = (float)lua_tonumber(L, -1);
	lua_pop(L, 1); // pop result of getfield
	return def;
}

int TextureManager::LoadPackage(const std::string &packageName, std::shared_ptr<FileSystem::File::Memory> file, FileSystem::IFileSystem &fs)
{
	TRACE("Loading texture package '%s'", packageName.c_str());

	lua_State *L = luaL_newstate();

	if ((luaL_loadbuffer(L, file->GetData(), file->GetSize(), packageName.c_str()) || lua_pcall(L, 0, 1, 0)) != 0)
	{
		TRACE("%s", lua_tostring(L, -1));
		lua_close(L);
		return 0;
	}

	if (!lua_istable(L, 1))
	{
		lua_close(L);
		return 0;
	}

	// loop over files
	for (lua_pushnil(L); lua_next(L, -2); lua_pop(L, 1))
	{
		// now 'key' is at index -2 and 'value' at index -1

		// check that value is a table
		if (!lua_istable(L, -1))
		{
			TRACE("WARNING: value is not a table; skipping.");
		}

		while (lua_istable(L, -1))
		{
			TexDescIterator td;

			// get a file name; load
			lua_getfield(L, -1, "file");
			std::string f = lua_tostring(L, -1);
			lua_pop(L, 1); // pop result of lua_getfield

			try
			{
				LoadTexture(td, f, fs);
			}
			catch (const std::exception &e)
			{
				TRACE("WARNING: could not load texture '%s' - %s", f.c_str(), e.what());
				break;
			}


			// get 'content' field
			lua_getfield(L, -1, "content");
			if (lua_istable(L, -1))
			{
				// loop over textures in 'content' table
				for (lua_pushnil(L); lua_next(L, -2); lua_pop(L, 1))
				{
					if (!lua_istable(L, -1))
					{
						TRACE("WARNING: element of 'content' is not a table; skipping");
						continue;
					}

					lua_pushvalue(L, -2); // create copy of the key
					if (const char *texname = lua_tostring(L, -1))
					{
						// now 'value' at index -2

						float scale_x = auxgetfloat(L, -2, "xscale", 1);
						float scale_y = auxgetfloat(L, -2, "yscale", 1);

						LogicalTexture tex;
						tex.dev_texture = td->id;

						// texture bounds
						float uvLeft = floorf(auxgetfloat(L, -2, "left", 0)) / (float)td->width;
						float uvRight = floorf(auxgetfloat(L, -2, "right", (float)td->width)) / (float)td->width;
						float uvTop = floorf(auxgetfloat(L, -2, "top", 0)) / (float)td->height;
						float uvBottom = floorf(auxgetfloat(L, -2, "bottom", (float)td->height)) / (float)td->height;

						// border
						tex.pxBorderSize = floorf(auxgetfloat(L, -2, "border", 0));
						float uvBorderWidth = tex.pxBorderSize / (float)td->width;
						float uvBorderHeight = tex.pxBorderSize / (float)td->height;

						// frames count
						int xframes = auxgetint(L, -2, "xframes", 1);
						int yframes = auxgetint(L, -2, "yframes", 1);

						// frame size with border
						float uvFrameWidth = (uvRight - uvLeft) / (float)xframes;
						float uvFrameHeight = (uvBottom - uvTop) / (float)yframes;

						// original size
						tex.pxFrameWidth = (float)td->width * scale_x * uvFrameWidth;
						tex.pxFrameHeight = (float)td->height * scale_y * uvFrameHeight;

						// pivot position
						tex.uvPivot.x = (float)auxgetfloat(L, -2, "xpivot", (float)td->width * uvFrameWidth / 2) / ((float)td->width * uvFrameWidth);
						tex.uvPivot.y = (float)auxgetfloat(L, -2, "ypivot", (float)td->height * uvFrameHeight / 2) / ((float)td->height * uvFrameHeight);

						// frames
						tex.uvFrames.reserve(xframes * yframes);
						for (int y = 0; y < yframes; ++y)
						{
							for (int x = 0; x < xframes; ++x)
							{
								math::RectFloat rt;
								rt.left = uvLeft + uvFrameWidth * (float)x + uvBorderWidth;
								rt.right = uvLeft + uvFrameWidth * (float)(x + 1) - uvBorderWidth;
								rt.top = uvTop + uvFrameHeight * (float)y + uvBorderHeight;
								rt.bottom = uvTop + uvFrameHeight * (float)(y + 1) - uvBorderHeight;
								tex.uvFrames.push_back(rt);
							}
						}

						//---------------------
						if (xframes > 0 && yframes > 0)
						{
							td->refCount++;
							//---------------------------------------------
							std::map<std::string, size_t>::iterator it =
								_mapName_to_Index.find(texname);

							if (_mapName_to_Index.end() != it)
							{
								// replace existing logical texture
								LogicalTexture &existing = _logicalTextures[it->second];
								TexDescIterator tmp =
									_mapDevTex_to_TexDescIter[existing.dev_texture];
								existing = tex;
								tmp->refCount--;
								assert(tmp->refCount >= 0);
							}
							else
							{
								// define new texture
								_mapName_to_Index[texname] = _logicalTextures.size();
								_logicalTextures.push_back(tex);
							}
						} // end if( xframes > 0 && yframes > 0 )
					} // end if( texname )
					lua_pop(L, 1); // remove copy of the key
				} // end loop over 'content'
			} // end if 'content' is table
			else
			{
				TRACE("WARNING: 'content' field is not a table.");
			}
			lua_pop(L, 1); // pop the result of getfield("content")
			break;
		} // end of while( lua_istable(L, -1) )
	}
	lua_close(L);


	//
	// unload unused textures
	//
	TexDescIterator it = _textures.begin();
	while (_textures.end() != it)
	{
		TexDescIterator tmp = it++;
		assert(tmp->refCount >= 0);
		if (0 == tmp->refCount)
			Unload(tmp);
	}

	TRACE("Total number of loaded textures: %d", _logicalTextures.size());
	return _logicalTextures.size();
}

int TextureManager::LoadDirectory(const std::string &dirName, const std::string &texPrefix, FileSystem::IFileSystem &fs)
{
	int count = 0;
	std::shared_ptr<FileSystem::IFileSystem> dir = fs.GetFileSystem(dirName);
	auto files = dir->EnumAllFiles("*.tga");
	for (auto it = files.begin(); it != files.end(); ++it)
	{
		TexDescIterator td;
		std::string fileName = dirName + '/' + *it;
		try
		{
			LoadTexture(td, fileName, fs);
		}
		catch (const std::exception &e)
		{
			TRACE("WARNING: could not load texture '%s' - %s", fileName.c_str(), e.what());
			continue;
		}

		std::string texName = texPrefix + *it;
		texName.erase(texName.length() - 4); // cut out the file extension

		LogicalTexture tex;
		tex.dev_texture = td->id;
		tex.uvPivot = Vector2(0.5f, 0.5f);
		tex.pxFrameWidth = (float)td->width;
		tex.pxFrameHeight = (float)td->height;
		tex.pxBorderSize = 0;

		math::RectFloat frame = { 0,0,1,1 };
		tex.uvFrames.push_back(frame);
		//---------------------
		if (_mapName_to_Index.end() != _mapName_to_Index.find(texName))
			continue; // skip if there is a texture with the same name
		_mapName_to_Index[texName] = _logicalTextures.size();
		_logicalTextures.push_back(tex);
		td->refCount++;
		count++;
	}
	return count;
}

size_t TextureManager::FindSprite(const std::string &name) const
{
	std::map<std::string, size_t>::const_iterator it = _mapName_to_Index.find(name);
	if (_mapName_to_Index.end() != it)
		return it->second;

	// flood the console
	TRACE("texture '%s' not found!", name.c_str());

	return 0; // index of checker texture
}

void TextureManager::GetTextureNames(std::vector<std::string> &names,
	const char *prefix, bool noPrefixReturn) const
{
	size_t trimLength = (prefix && noPrefixReturn) ? std::strlen(prefix) : 0;

	names.clear();
	std::map<std::string, size_t>::const_iterator it = _mapName_to_Index.begin();
	for (; it != _mapName_to_Index.end(); ++it)
	{
		if (prefix && 0 != it->first.find(prefix))
			continue;
		names.push_back(it->first.substr(trimLength));
	}
}

float TextureManager::GetCharHeight(size_t fontTexture) const
{
	return GetSpriteInfo(fontTexture).pxFrameHeight;
}

// end of file
