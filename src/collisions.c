#include "collisions.h"

extern int mapTileAt(int, int);
extern int maxMapX(void);
extern Entity *getPlayer(void);

int collision(int, int, int, int, int, int, int, int);

void doCollisions()
{
	int i, j;
	Entity *player = getPlayer();

	for (i=0;i<MAX_ENTITIES;i++)
	{
		if (entity[i].active == ACTIVE && !(entity[i].flags & INVULNERABLE))
		{
			if (!(player->flags & INVULNERABLE) && collision(entity[i].x, entity[i].y, entity[i].w, entity[i].h, player->x, player->y, player->w, player->h) == 1)
			{
				if (entity[i].touch != NULL)
				{
					self = &entity[i];
					
					self->touch(player);
				}
			}
			
			for (j=0;j<MAX_ENTITIES;j++)
			{
				if (i != j && entity[j].active == ACTIVE && !(entity[j].flags & INVULNERABLE))
				{
					if (collision(entity[i].x, entity[i].y, entity[i].w, entity[i].h, entity[j].x, entity[j].y, entity[j].w, entity[j].h) == 1)
					{
						if (entity[i].touch != NULL)
						{
							self = &entity[i];
							
							self->touch(&entity[j]);
						}
					}
				}
			}
		}
	}
}

Entity *isSpaceEmpty(Entity *e)
{
	int i;
	Entity *player = getPlayer();
	
	if (player->active == ACTIVE && collision(e->x, e->y, e->w, e->h, player->x, player->y, player->w, player->h) == 1)
	{
		return player;
	}

	for (i=0;i<MAX_ENTITIES;i++)
	{
		if (entity[i].active == ACTIVE && collision(e->x, e->y, e->w, e->h, entity[i].x, entity[i].y, entity[i].w, entity[i].h) == 1)
		{
			return &entity[i];
		}
	}
	
	return NULL;
}

void checkToMap(Entity *e)
{
	int i, x1, x2, y1, y2;
	
	/* Remove the entity from the ground */
	
	e->flags &= ~ON_GROUND;
	
	/* Test the horizontal movement first */
	
	i = e->h > TILE_SIZE ? TILE_SIZE : e->h;
	
	for (;;)
	{
		x1 = (e->x + e->dirX) / TILE_SIZE;
		x2 = (e->x + e->dirX + e->w - 1) / TILE_SIZE;
	
		y1 = (e->y) / TILE_SIZE;
		y2 = (e->y + i - 1) / TILE_SIZE;
		
		if (x1 >= 0 && x2 < MAX_MAP_X && y1 >= 0 && y2 < MAX_MAP_Y)
		{
			if (e->dirX > 0)
			{
				/* Trying to move right */
				
				if (mapTileAt(x2, y2) == SLOPE_UP)
				{
					if (i == e->h)
					{
						e->y -= e->dirX;
						
						e->dirY = 0;
						
						e->flags |= ON_GROUND;
					}
				}
				
				else if (mapTileAt((e->x + e->w - 1) / TILE_SIZE, y2) == SLOPE_UP)
				{
					if (i == e->h)
					{
						e->y = y2* TILE_SIZE;
						
						e->y -= e->h;
						
						e->dirY = 0;
						
						e->flags |= ON_GROUND;
					}
				}
				
				else if ((mapTileAt(x2, y1) != BLANK_TILE) || (mapTileAt(x2, y2) != BLANK_TILE))
				{
					/* Place the player as close to the solid tile as possible */
		
					e->x = x2 * TILE_SIZE;
					
					e->x -= e->w + 1;
		
					e->dirX = 0;
				}
			}
		
			else if (e->dirX < 0)
			{
				/* Trying to move left */
				
				if (mapTileAt(x1, y2) == SLOPE_DOWN)
				{
					if (i == e->h)
					{
						e->y += e->dirX;
						
						e->dirY = 0;
						
						e->flags |= ON_GROUND;
					}
				}
				
				else if (mapTileAt((e->x) / TILE_SIZE, y2) == SLOPE_DOWN)
				{
					if (i == e->h)
					{
						e->y = y2* TILE_SIZE;
						
						e->y -= e->h;
						
						e->dirY = 0;
						
						e->flags |= ON_GROUND;
					}
				}
		
				else if ((mapTileAt(x1, y1) != BLANK_TILE) || (mapTileAt(x1, y2) != BLANK_TILE))
				{
					/* Place the player as close to the solid tile as possible */
					
					e->x = (x1 + 1) * TILE_SIZE;
		
					e->dirX = 0;
				}
			}
		}
		
		/* Exit this loop if we have tested all of the body */
		
		if (i == e->h)
		{
			break;
		}
		
		/* Test the next block */
		
		i += TILE_SIZE;
		
		if (i > e->h)
		{
			i = e->h;
		}
	}

	/* Now test the vertical movement */
	
	i = e->w > TILE_SIZE ? TILE_SIZE : e->w;
	
	for (;;)
	{
		x1 = (e->x) / TILE_SIZE;
		x2 = (e->x + i) / TILE_SIZE;
	
		y1 = (e->y + e->dirY) / TILE_SIZE;
		y2 = (e->y + e->dirY + e->h) / TILE_SIZE;
		
		if (x1 >= 0 && x2 < MAX_MAP_X && y1 >= 0 && y2 < MAX_MAP_Y)
		{
			if (e->dirY > 0)
			{
				/* Trying to move down */
				
				if (mapTileAt(x2, y2) == SLOPE_UP)
				{
					if (i == e->w)
					{
						e->y = (y2 + 1) * TILE_SIZE;
						e->y -= e->h;
						
						e->y -= ((int)e->x + e->w) % TILE_SIZE;
						
						e->dirY = 0;
						
						e->flags |= ON_GROUND;
					}
				}
				
				else if (mapTileAt(x1, y2) == SLOPE_DOWN)
				{
					if (i == e->w)
					{
						e->y = (y2 + 1) * TILE_SIZE;
						e->y -= e->h;
						
						e->y -= TILE_SIZE - ((int)e->x) % TILE_SIZE;
						
						e->dirY = 0;
						
						e->flags |= ON_GROUND;
					}
				}
				
				else if ((mapTileAt(x1, y2) != BLANK_TILE) || (mapTileAt(x2, y2) != BLANK_TILE))
				{
					/* Place the player as close to the solid tile as possible */
					
					e->y = y2 * TILE_SIZE;
					e->y -= e->h;
		
					e->dirY = 0;
					
					e->flags |= ON_GROUND;
				}
			}
		
			else if (e->dirY < 0)
			{
				/* Trying to move up */
		
				if ((mapTileAt(x1, y1) != BLANK_TILE) || (mapTileAt(x2, y1) != BLANK_TILE))
				{
					/* Place the player as close to the solid tile as possible */
		
					e->y = (y1 + 1) * TILE_SIZE;
		
					e->dirY = 0;
				}
			}
		}
		
		if (i == e->w)
		{
			break;
		}
		
		i += TILE_SIZE;
		
		if (i > e->w)
		{
			i = e->w;
		}
	}

	/* Now apply the movement */

	e->x += e->dirX;
	e->y += e->dirY;
	
	if (e->x < 0)
	{
		e->x = 0;
		
		e->dirX = 0;
	}
	
	else if (e->x + e->w >= maxMapX())
	{
		e->x = maxMapX() - e->w - 1;
		
		e->dirX = 0;
	}
}

int isValidOnMap(Entity *e)
{
	int i, x1, x2, y1, y2;
	
	i = e->w > TILE_SIZE ? TILE_SIZE : e->w;
	
	for (;;)
	{
		x1 = (e->x) / TILE_SIZE;
		x2 = (e->x + i - 1) / TILE_SIZE;
	
		y1 = (e->y) / TILE_SIZE;
		y2 = (e->y + e->h - 1) / TILE_SIZE;
		
		if (mapTileAt(x1, y1) != BLANK_TILE || mapTileAt(x2, y1) != BLANK_TILE ||
			mapTileAt(x1, y2) != BLANK_TILE || mapTileAt(x2, y2) != BLANK_TILE)
		{
			return 0;
		}
		
		if (i == e->w)
		{
			break;
		}
		
		i += e->w;
		
		if (i > e->w)
		{
			i = e->w;
		}
	}
	
	i = e->h > TILE_SIZE ? TILE_SIZE : e->h;
	
	for (;;)
	{
		x1 = (e->x) / TILE_SIZE;
		x2 = (e->x + e->w - 1) / TILE_SIZE;
	
		y1 = (e->y) / TILE_SIZE;
		y2 = (e->y + i - 1) / TILE_SIZE;
		
		if (mapTileAt(x1, y1) != BLANK_TILE || mapTileAt(x2, y1) != BLANK_TILE ||
			mapTileAt(x1, y2) != BLANK_TILE || mapTileAt(x2, y2) != BLANK_TILE)
		{
			return 0;
		}
		
		if (i == e->h)
		{
			break;
		}
		
		i += e->h;
		
		if (i > e->h)
		{
			i = e->h;
		}
	}
	
	return 1;
}

/* Very standard 2D collision detection routine */

int collision(int x0, int y0, int w0, int h0, int x2, int y2, int w1, int h1)
{
	int x1 = x0 + w0 - 1;
	int y1 = y0 + h0 - 1;

	int x3 = x2 + w1 - 1;
	int y3 = y2 + h1 - 1;

	return !(x1<x2 || x3<x0 || y1<y2 || y3<y0);
}