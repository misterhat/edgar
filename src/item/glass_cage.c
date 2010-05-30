/*
Copyright (C) 2009-2010 Parallel Realities

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "../headers.h"

#include "../graphics/animation.h"
#include "../audio/audio.h"
#include "../system/properties.h"
#include "../entity.h"
#include "../item/key_items.h"
#include "../item/item.h"
#include "../system/error.h"
#include "../system/random.h"
#include "../collisions.h"
#include "../custom_actions.h"

extern Entity *self;

static void activate(int);
static void wait(void);
static void die(void);
static void closeFinish(void);
static void takeDamage(Entity *, int);

Entity *addGlassCage(int x, int y, char *name)
{
	Entity *e = getFreeEntity();

	if (e == NULL)
	{
		showErrorAndExit("No free slots to add a Glass Cage");
	}

	loadProperties(name, e);

	e->x = x;
	e->y = y;

	e->type = KEY_ITEM;

	e->face = LEFT;

	e->action = &wait;
	e->touch = &entityTouch;
	e->activate = &activate;
	e->takeDamage = &takeDamage;

	e->draw = &drawLoopingAnimationToMap;

	setEntityAnimation(e, STAND);

	return e;
}

static void activate(int val)
{
	if (val == 100)
	{
		self->active = TRUE;
	}
}

static void wait()
{
	if (self->y > self->startY)
	{
		self->layer = BACKGROUND_LAYER;
		
		self->y -= 2;
		
		if (self->y <= self->startY)
		{
			self->layer = FOREGROUND_LAYER;
			
			self->y = self->startY;
			
			setEntityAnimation(self, WALK);
			
			self->animationCallback = &closeFinish;
		}
	}
	
	if (self->target != NULL)
	{
		self->target->x = self->x + self->w / 2 - self->target->w / 2;
		
		self->target->flags &= ~ON_GROUND;
	}
	
	if (self->active == TRUE)
	{
		self->thinkTime--;
		
		if (self->thinkTime <= 0)
		{
			playSoundToMap("sound/common/shatter.ogg", -1, self->x, self->y, 0);

			self->action = &die;
		}
	}
}

static void die()
{
	int i;
	Entity *e;

	for (i=0;i<7;i++)
	{
		e = addTemporaryItem("item/glass_wall_piece", self->x, self->y, RIGHT, 0, 0);

		e->x += self->w / 2 - e->w / 2;
		e->y += self->h / 2 - e->h / 2;

		e->dirX = (prand() % 10) * (prand() % 2 == 0 ? -1 : 1);
		e->dirY = ITEM_JUMP_HEIGHT + (prand() % ITEM_JUMP_HEIGHT);

		setEntityAnimation(e, i);

		e->thinkTime = 60 + (prand() % 60);
	}
	
	self->head->mental = 0;

	self->inUse = FALSE;
}

static void takeDamage(Entity *other, int damage)
{
	setCustomAction(self, &invulnerableNoFlash, 20, 0, 0);

	playSoundToMap("sound/common/dink.ogg", 2, self->x, self->y, 0);
}

static void closeFinish()
{
	setEntityAnimation(self, JUMP);
}