#include "weather.h"

extern void drawBox(int, int, int, int, int, int, int);
extern void clearScreen(int, int, int);
extern int mapTileAt(int, int);

static void initLightRain(void);
static void initHeavyRain(void);
static void initStorm(void);
static void initSnow(void);
static void rain(void);
static void snow(void);
static void storm(void);
static void drawRain(void);
static void drawSnow(void);

void setWeather(int weatherType)
{
	switch (weatherType)
	{
		case LIGHT_RAIN:
			game.weatherAction = &initLightRain;
			game.weatherDraw = &drawRain;
		break;
		
		case HEAVY_RAIN:
			game.weatherAction = &initHeavyRain;
			game.weatherDraw = &drawRain;
		break;
		
		case STORMY:
			game.weatherAction = &initStorm;
			game.weatherDraw = &drawRain;
		break;
		
		case SNOW:
			game.weatherAction = &initSnow;
			game.weatherDraw = &drawSnow;
		break;
	}
	
	game.weatherThinkTime = 60;
}

static void initLightRain()
{
	int i;
	
	memset(droplet, 0, sizeof(Droplet) * MAX_DROPS);
	
	for (i=0;i<MAX_DROPS/2;i++)
	{
		droplet[i].x = rand() % SCREEN_WIDTH;
		droplet[i].y = rand() % SCREEN_HEIGHT;
		
		droplet[i].dirX = 0;
		droplet[i].dirY = 8 + rand() % 8;
		
		droplet[i].active = ACTIVE;
	}
	
	game.weatherAction = &rain;
}

static void initHeavyRain()
{
	int i;
	
	memset(droplet, 0, sizeof(Droplet) * MAX_DROPS);
	
	for (i=0;i<MAX_DROPS;i++)
	{
		droplet[i].x = rand() % SCREEN_WIDTH;
		droplet[i].y = rand() % SCREEN_HEIGHT;
		
		droplet[i].dirX = 0;
		droplet[i].dirY = 8 + rand() % 8;
		
		droplet[i].active = ACTIVE;
	}
	
	game.weatherAction = &rain;
}

static void rain()
{
	int i;
	
	for (i=0;i<MAX_DROPS;i++)
	{
		if (droplet[i].active == ACTIVE)
		{
			droplet[i].x += droplet[i].dirX;
			droplet[i].y += droplet[i].dirY;
			
			if (droplet[i].y >= SCREEN_HEIGHT || mapTileAt(droplet[i].x / TILE_SIZE, droplet[i].y / TILE_SIZE) != BLANK_TILE)
			{
				droplet[i].x = rand() % SCREEN_WIDTH;
				droplet[i].y = -8 - rand() % 20;
				
				droplet[i].dirX = 0;
				droplet[i].dirY = 8 + rand() % 8;
			}
		}
	}
}

static void initStorm()
{
	int i;
	
	memset(droplet, 0, sizeof(Droplet) * MAX_DROPS);
	
	for (i=0;i<MAX_DROPS;i++)
	{
		droplet[i].x = rand() % SCREEN_WIDTH;
		droplet[i].y = rand() % SCREEN_HEIGHT;
		
		droplet[i].dirX = 0;
		droplet[i].dirY = 8 + rand() % 8;
		
		droplet[i].active = ACTIVE;
	}
	
	game.weatherAction = &storm;
}

static void storm()
{
	rain();
	
	game.weatherThinkTime--;
	
	if (game.weatherThinkTime < -60)
	{
		game.weatherThinkTime = 600 + rand() % 1200;
	}
}

static void initSnow()
{
	int i;
	
	memset(droplet, 0, sizeof(Droplet) * MAX_DROPS);
	
	for (i=0;i<MAX_DROPS;i++)
	{
		droplet[i].x = rand() % SCREEN_WIDTH;
		droplet[i].y = rand() % SCREEN_HEIGHT;
		
		droplet[i].dirX = 0.1f * (rand() % 20) - 0.1f * (rand() % 20);
		droplet[i].dirY = 01.f + 0.1f * (rand() % 10);
		
		droplet[i].active = ACTIVE;
	}
	
	game.weatherAction = &snow;
}

static void snow()
{
	int i;
	
	for (i=0;i<MAX_DROPS;i++)
	{
		droplet[i].x += droplet[i].dirX;
		droplet[i].y += droplet[i].dirY;
		
		if (rand() % 30 == 0)
		{
			droplet[i].dirX = 0.1f * (rand() % 20) - 0.1f * (rand() % 20);
		}
		
		if (droplet[i].y >= SCREEN_HEIGHT || mapTileAt(droplet[i].x / TILE_SIZE, droplet[i].y / TILE_SIZE) != BLANK_TILE)
		{
			droplet[i].x = rand() % SCREEN_WIDTH;
			droplet[i].y = -8 - rand() % 20;
			
			droplet[i].dirX = 0.1f * (rand() % 20) - 0.1f * (rand() % 20);
			droplet[i].dirY = 01.f + 0.1f * (rand() % 10);
		}
	}
}

static void drawRain()
{
	int i;
	
	for (i=0;i<MAX_DROPS;i++)
	{
		if (droplet[i].active == ACTIVE)
		{
			drawBox(droplet[i].x, droplet[i].y, 1, 8, 255, 255, 255);
		}
	}
	
	if (game.weatherThinkTime >= -10 && game.weatherThinkTime < 0)
	{
		clearScreen(255, 255, 255);
	}
}

static void drawSnow()
{
	int i;
	
	for (i=0;i<MAX_DROPS;i++)
	{
		if (droplet[i].active == ACTIVE)
		{
			drawBox(droplet[i].x, droplet[i].y, 2, 2, 255, 255, 255);
		}
	}
}