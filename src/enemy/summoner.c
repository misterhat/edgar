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
#include "../geometry.h"
#include "../item/item.h"

extern Entity *self, player;
extern Game game;

static void lookForPlayer(void);
static void summon(void);
static void summonWait(void);
static void hover(void);
static void summonEnd(void);
static void die(void);

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
	e->die = &die;
	e->takeDamage = &entityTakeDamageNoFlinch;
	e->reactToBlock = &changeDirection;
	e->touch = &entityTouch;

	e->type = ENEMY;

	setEntityAnimation(e, STAND);

	return e;
}

static void lookForPlayer()
{
	float dirX;

	self->thinkTime--;

	if (self->dirX == 0)
	{
		self->dirX = self->face == LEFT ? self->speed : -self->speed;
	}

	self->face = self->dirX > 0 ? RIGHT : LEFT;

	dirX = self->dirX;

	checkToMap(self);

	if (self->dirX == 0 && dirX != 0)
	{
		self->dirX = self->face == LEFT ? self->speed : -self->speed;

		self->face = self->face == LEFT ? RIGHT : LEFT;
	}

	if (self->thinkTime <= 0 && player.health > 0 && prand() % 30 == 0)
	{
		self->thinkTime = 0;

		if (collision(self->x + (self->face == RIGHT ? self->w : -160), self->y, 160, 200, player.x, player.y, player.w, player.h) == 1)
		{
			self->action = &summonWait;

			setEntityAnimation(self, ATTACK_1);

			self->animationCallback = &summon;

			self->dirX = 0;
		}
	}

	hover();
}

static void summonWait()
{
	checkToMap(self);
	
	hover();
}

static void summon()
{
	char summonList[MAX_VALUE_LENGTH], enemyToSummon[MAX_VALUE_LENGTH];
	char *token;
	int summonIndex = 0, summonCount = 0;
	Entity *e;

	STRNCPY(summonList, self->requires, MAX_VALUE_LENGTH);

	token = strtok(summonList, "|");

	while (token != NULL)
	{
		token = strtok(NULL, "|");

		summonCount++;
	}

	if (summonCount == 0)
	{
		printf("Summoner at %f %f has no summon list\n", self->x, self->y);

		exit(1);
	}

	summonIndex = prand() % summonCount;

	STRNCPY(summonList, self->requires, MAX_VALUE_LENGTH);

	summonCount = 0;

	token = strtok(summonList, "|");

	while (token != NULL)
	{
		if (summonCount == summonIndex)
		{
			break;
		}

		token = strtok(NULL, "|");

		summonCount++;
	}

	snprintf(enemyToSummon, MAX_VALUE_LENGTH, "enemy/%s", token);

	e = addEnemy(enemyToSummon, self->x, self->y);

	e->targetX = self->x;

	e->targetY = self->y;

	e->x = e->targetX;

	e->y = e->targetY;

	calculatePath(e->x, e->y, e->targetX, e->targetY, &e->dirX, &e->dirY);

	e->flags |= (NO_DRAW|HELPLESS|TELEPORTING);

	self->action = &summonWait;

	setEntityAnimation(self, ATTACK_2);

	self->animationCallback = &summonEnd;
}

static void hover()
{
	self->startX++;

	if (self->startX >= 360)
	{
		self->startX = 0;
	}

	self->y = self->startY + sin(DEG_TO_RAD(self->startX)) * 8;
}

static void summonEnd()
{
	setEntityAnimation(self, STAND);
	
	self->action = &lookForPlayer;
	
	self->dirX = self->face == LEFT ? -self->speed : self->speed;

	self->thinkTime = 600;
}

static void die()
{
	Entity *e;

	if (prand() % 3 == 0)
	{
		e = dropCollectableItem("item/summoner_staff", self->x + self->w / 2, self->y, self->face);

		e->x -= e->w / 2;
	}

	entityDie();
}
