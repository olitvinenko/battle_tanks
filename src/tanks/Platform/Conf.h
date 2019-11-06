#pragma once

#define DIR_SCRIPTS      "scripts"
#define DIR_SAVE         "save"
#define DIR_MAPS         "maps"
#define DIR_SKINS        "skins"
#define DIR_THEMES       "themes"
#define DIR_MUSIC        "music"
#define DIR_SOUND        "sounds"
#define DIR_SCREENSHOTS  "screenshots"

#define FILE_TEXTURES    DIR_SCRIPTS"/textures.lua"
#define FILE_STARTUP     DIR_SCRIPTS"/init.lua"

#define TXT_VERSION      "Battle tanks (v0.0.1)"

#define LEVEL_MINSIZE   16
#define LEVEL_MAXSIZE   512
#define CELL_SIZE       32             // cell size in pixels
#define LOCATION_SIZE   (CELL_SIZE*4)  // location size in pixels (should be at least
                                       // as large as the largest sprite object)
#define VERSION    0x1520
