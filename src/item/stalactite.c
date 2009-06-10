/*
Copyright (C) 2009 Parallel Realities

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
#include "../custom_actions.h"
#include "../collisions.h"
#include "../enemy/rock.h"

extern Entity *self;

static void touch(Entity *);
static void takeDamage(Entity *, int);
static void die(void);
static void respawn(void);
static void wait(void);

Entity *addStalactite(int x, int y, char *name)
{
	Entity *e = getFreeEntity();

	if (e == NULL)
	{
		printf("No free slots to add a Stalactite\n");

		exit(1);
	}

	loadProperties(name, e);

	e->x = x;
	e->y = y;

	e->type = KEY_ITEM;

	e->face = RIGHT;

	e->action = &wait;
	e->touch = &touch;
	e->die = &die;

	e->draw = &drawLoopingAnimationToMap;

	setEntityAnimation(e, STAND);

	return e;
}

static void touch(Entity *other)
{
	if (self->active == TRUE && (other->flags & ATTACKING) && !(self->flags & INVULNERABLE))
	{
		takeDamage(other, other->damage);
	}

	if (self->inUse == TRUE)
	{
		pushEntity(other);
	}
}

static void wait()
{
	checkToMap(self);
}

static void takeDamage(Entity *other, int damage)
{
	if (strcmpignorecase(self->requires, other->name) == 0 || other->type == PROJECTILE)
	{
		self->health -= damage;

		setCustomAction(self, &flashWhite, 6, 0);
		setCustomAction(self, &invulnerableNoFlash, 20, 0);

		if (self->health <= 0)
		{
			if (self->thinkTime == 0)
			{
				self->flags &= ~FLY;

				self->health = self->maxHealth;

				self->thinkTime = 1;
			}

			else
			{
				self->thinkTime = 300;

				self->die();
			}
		}
	}

	else
	{
		setCustomAction(self, &invulnerableNoFlash, 20, 0);

		playSound("sound/common/dink.ogg", -1, self->x, self->y, 0);
	}
}

static void die()
{
	Entity *e;

	self->flags |= NO_DRAW;

	self->touch = NULL;

	self->action = &respawn;

	e = addSmallRock(self->x, self->y, "common/small_rock");

	e->x += (self->w - e->w) / 2;
	e->y += (self->h - e->h) / 2;

	e->dirX = -3;
	e->dirY = -8;

	e = addSmallRock(self->x, self->y, "common/small_rock");

	e->x += (self->w - e->w) / 2;
	e->y += (self->h - e->h) / 2;

	e->dirX = 3;
	e->dirY = -8;
}

static void respawn()
{
	self->thinkTime--;

	if (self->thinkTime == 0)
	{
		self->flags &= ~NO_DRAW;

		self->flags |= FLY;

		self->action = &wait;

		self->health = self->maxHealth;

		self->touch = &touch;

		setCustomAction(self, &invulnerable, 180, 0);

		self->x = self->startX;

		self->y = self->startY;

		self->dirX = self->dirY = 0;
	}
}