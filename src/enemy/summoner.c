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
#include "../system/properties.h"
#include "../entity.h"
#include "../collisions.h"
#include "../system/random.h"
#include "../audio/audio.h"
#include "enemies.h"
#include "../custom_actions.h"
#include "../hud.h"
#include "../player.h"

extern Entity *self, player;
extern Game game;

static void lookForPlayer(void);
static void summon(void);
static void summonWait(void);
static void hover(void);

Entity *addSummoner(int x, int y, char *name)
{
	Entity *e = getFreeEntity();

	if (e == NULL)
	{
		printf("No free slots to add a Summoner\n");

		exit(1);
	}

	loadProperties(name, e);

	e->x = x;
	e->y = y;

	e->action = &lookForPlayer;
	e->draw = &drawLoopingAnimationToMap;
	e->die = &entityDieNoDrop;
	e->pain = NULL;
	e->takeDamage = &entityTakeDamageFlinch;
	e->reactToBlock = NULL;
	e->touch = &entityTouch;

	e->type = ENEMY;

	setEntityAnimation(e, STAND);

	return e;
}

static void lookForPlayer()
{
	self->thinkTime--;

	if (self->thinkTime <= 0)
	{
		switch (prand() % 5)
		{
			case 0:
			case 1:
				self->dirX = self->speed;
			break;

			case 2:
			case 3:
				self->dirX = -self->speed;
			break;

			default:
				self->dirX = 0;
			break;
		}

		self->thinkTime = 180 + prand() % 120;
	}

	if (self->dirX < 0)
	{
		self->face = LEFT;
	}

	else if (self->dirX > 0)
	{
		self->face = RIGHT;
	}

	self->dirY = 0;

	checkToMap(self);

	if (self->dirX == 0)
	{
		self->dirX = self->face == LEFT ? self->speed : -self->speed;

		self->face = self->face == LEFT ? RIGHT : LEFT;
	}

	if (player.health > 0 && prand() % 30 == 0)
	{
		if (collision(self->x + (self->face == RIGHT ? self->w : -160), self->y, 160, 200, player.x, player.y, player.w, player.h) == 1)
		{
			self->action = &summonWait;

			setEntityAnimation(self, ATTACK_1);

			self->animationCallback = &summon;

			self->dirX = 0;
		}
	}
}

static void summonWait()
{
	hover();

	checkToMap(self);
}

static void summon()
{
	char summonList[MAX_VALUE_LENGTH], enemyToSummon[MAX_VALUE_LENGTH];
	char *token;
	int summonIndex = 0, summonCount = 0;

	STRNCPY(summonList, self->requires, MAX_VALUE_LENGTH);

	token = strtok(summonList, "|");

	while (token != NULL)
	{
		token = strtok(NULL, "|");

		summonCount++;
	}

	if (summonCount == 0)
	{
		printf("Summoner at %f %f has no summon list\n", self->endX, self->endY);

		exit(1);
	}

	summonIndex = prand() % summonCount;

	STRNCPY(summonList, self->requires, MAX_VALUE_LENGTH);

	summonCount = 0;

	token = strtok(summonList, "|");

	while (token != NULL)
	{
		if (summonIndex == summonIndex)
		{
			break;
		}

		token = strtok(NULL, "|");

		summonCount++;
	}

	snprintf(enemyToSummon, MAX_VALUE_LENGTH, "enemy/%s", token);

	printf("Summoning %s\n", enemyToSummon);

	addEnemy(enemyToSummon, self->x, self->y);

	self->action = &summonWait;

	setEntityAnimation(self, ATTACK_2);

	self->animationCallback = &lookForPlayer;
}

static void hover()
{
	self->startX++;

	if (self->startX >= 360)
	{
		self->startX = 0;
	}

	self->y = self->startY + sin(DEG_TO_RAD(self->startX)) * 16;
}
