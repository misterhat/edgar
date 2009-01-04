#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_mixer.h"
#include "SDL/SDL_ttf.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define MAX_MAP_X 400
#define MAX_MAP_Y 300

#define TILE_SIZE 32

#define GRAVITY_SPEED 1

#define MAX_FALL_SPEED 20

#define JUMP_HEIGHT 11

#define PLAYER_SPEED 2.5

#define TRANS_R 127
#define TRANS_G 0
#define TRANS_B 127

#define MAX_ANIMATIONS 255

#define MAX_ENTITIES 256

#define BLANK_TILE 0

#define MAX_TILES 255

#define MAX_LINE_LENGTH 1024

#define PI 3.14159265

#define MAX_MESSAGE_LENGTH 100

#define MAX_INVENTORY_ITEMS 24

#define MAX_DROPS 240

#define MAX_SPRITES 4096

#define MAX_PROPS_ENTRIES 20

#define MAX_PROPS_FILES 30

#define MAX_CUSTOM_ACTIONS 4

#define MAX_AMBIENT_SOUNDS 10

#define SLOPE_UP 200

#define SLOPE_DOWN 201

#ifndef PROD
	#define INSTALL_PATH ""
#else
	#define INSTALL_PATH "/usr/share/games/edgar/"
#endif

#ifdef WINDOWS
	#define strcmpignorecase(x, y) stricmp(x, y)
#else
	#define strcmpignorecase(x, y) strcasecmp(x, y)
#endif

#define MAX_SOUNDS 256

#define ITEM_JUMP_HEIGHT -6

enum
{
	LEFT,
	RIGHT
};

enum
{
	INACTIVE,
	ACTIVE
};

enum
{
	ON_GROUND = 1,
	NO_DRAW = 2,
	PUSHABLE = 4,
	HELPLESS = 8,
	INVULNERABLE = 16,
	BURNING = 32,
	FROZEN = 64,
	ELECTRIFIED = 128,
	STATIC = 256,
	FLY = 512
};

enum
{
	PLAYER,
	WEAPON,
	ITEM,
	KEY_ITEM,
	ENEMY,
	LIFT,
	HEALTH,
	SHIELD
};

enum
{
	TILES,
	ENTITIES
};

enum
{
	STAND,
	WALK,
	JUMP,
	PAIN,
	DIE,
	ATTACK_1,
	ATTACK_2,
	ATTACK_3,
	ATTACK_4,
	ATTACK_5,
	MAX_ANIMATION_TYPES
};

enum
{
	NO_WEATHER,
	LIGHT_RAIN,
	HEAVY_RAIN,
	STORMY,
	SNOW,
	MAX_WEATHER_TYPES
};