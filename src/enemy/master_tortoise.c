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
#include "../system/properties.h"
#include "../entity.h"
#include "../collisions.h"
#include "../system/random.h"
#include "../audio/audio.h"
#include "../custom_actions.h"
#include "../system/error.h"
#include "../game.h"
#include "../hud.h"
#include "../map.h"
#include "../projectile.h"
#include "../item/item.h"

extern Entity *self, player;

static void walk(void);
static void entityWait(void);
static void changeWalkDirectionStart(void);
static void changeWalkDirection(void);
static void changeWalkDirectionFinish(void);
static void electrifyStart(void);
static void electrify(void);
static void electrifyFinish(void);
static void createElectricity(void);
static void takeDamage(Entity *, int);
static void doElectricity(void);
static void iceAttackStart(void);
static void iceAttack(void);
static void createIce(void);
static void iceAttackFinish(void);
static void iceBallMove(void);
static void iceFloorWait(void);
static void iceSpikeMove(void);
static void spikeTakeDamage(Entity *, int);
static void breatheFireInit(void);
static void breatheFireWait(void);
static void init(void);
static void becomeRampaging(void);

Entity *addMasterTortoise(int x, int y, char *name)
{
	Entity *e = getFreeEntity();

	if (e == NULL)
	{
		showErrorAndExit("No free slots to add a Master Tortoise");
	}

	loadProperties(name, e);

	e->x = x;
	e->y = y;

	e->action = &init;

	e->draw = &drawLoopingAnimationToMap;
	e->touch = &entityTouch;
	e->die = &entityDie;
	e->takeDamage = &takeDamage;
	e->reactToBlock = &changeDirection;

	e->type = ENEMY;

	setEntityAnimation(e, STAND);

	return e;
}

static void init()
{
	switch (self->mental)
	{
		case 0: /* Lightning */
			setEntityAnimation(self, STAND);
		break;

		case 1: /* Ice */
			setEntityAnimation(self, WALK);
		break;

		default: /* Fire */
			setEntityAnimation(self, JUMP);
		break;
	}

	self->action = &walk;
}

static void walk()
{
	moveLeftToRight();

	self->thinkTime--;

	if (self->health >= self->maxHealth + 200)
	{
		self->action = &becomeRampaging;

		return;
	}

	switch (self->mental)
	{
		case 0: /* Lightning */
			if (self->thinkTime <= 0)
			{
				self->dirX = 0;

				if (prand() % 3 == 0)
				{
					self->action = &changeWalkDirectionStart;
				}

				else
				{
					self->thinkTime = 60;

					self->action = &electrifyStart;
				}
			}
		break;

		case 1: /* Ice */
			if (self->thinkTime <= 0)
			{
				self->dirX = 0;

				if (prand() % 3 == 0)
				{
					self->action = &changeWalkDirectionStart;
				}

				else
				{
					self->thinkTime = 60;

					self->action = &iceAttackStart;
				}
			}
		break;

		default: /* Fire */
			if (player.health > 0 && prand() % 60 == 0)
			{
				if (collision(self->x + (self->face == RIGHT ? self->w : -320), self->y, 320, self->h, player.x, player.y, player.w, player.h) == 1)
				{
					self->action = &breatheFireInit;

					self->dirX = 0;
				}
			}

			else if (self->thinkTime <= 0)
			{
				self->dirX = 0;

				self->action = &changeWalkDirectionStart;
			}
		break;
	}
}

static void changeWalkDirectionStart()
{
	switch (self->mental)
	{
		case 0: /* Lightning */
			setEntityAnimation(self, CUSTOM_1);
		break;

		case 1: /* Ice */
			setEntityAnimation(self, CUSTOM_2);
		break;

		default: /* Fire */
			setEntityAnimation(self, CUSTOM_3);
		break;
	}

	self->action = &entityWait;

	self->animationCallback = &changeWalkDirection;

	self->thinkTime = 60;

	checkToMap(self);
}

static void changeWalkDirection()
{
	self->thinkTime--;

	self->action = &changeWalkDirection;

	setEntityAnimation(self, CUSTOM_4);

	if (self->thinkTime <= 0)
	{
		/* Change the type of enemy that you are */

		if (prand() % 2 == 0)
		{
			self->mental = prand() % 3;
		}

		self->face = self->face == LEFT ? RIGHT : LEFT;

		self->frameSpeed = -1;

		switch (self->mental)
		{
			case 0: /* Lightning */
				setEntityAnimation(self, CUSTOM_1);
			break;

			case 1: /* Ice */
				setEntityAnimation(self, CUSTOM_2);
			break;

			default: /* Fire */
				setEntityAnimation(self, CUSTOM_3);
			break;
		}

		self->animationCallback = &changeWalkDirectionFinish;

		self->action = &entityWait;
	}

	checkToMap(self);
}

static void changeWalkDirectionFinish()
{
	self->frameSpeed = 1;

	switch (self->mental)
	{
		case 0: /* Lightning */
			setEntityAnimation(self, STAND);
		break;

		case 1: /* Ice */
			setEntityAnimation(self, WALK);
		break;

		default: /* Fire */
			setEntityAnimation(self, JUMP);
		break;
	}

	self->action = &walk;

	self->dirX = self->face == LEFT ? -self->speed : self->speed;

	self->thinkTime = 120 + prand() % 120;

	checkToMap(self);
}

static void entityWait()
{
	checkToMap(self);
}

static void electrifyStart()
{
	self->dirX = 0;

	self->frameSpeed = 0;

	self->thinkTime--;

	if (self->thinkTime <= 0)
	{
		self->frameSpeed = 1;

		setEntityAnimation(self, CUSTOM_1);

		self->animationCallback = &createElectricity;
	}

	checkToMap(self);
}

static void createElectricity()
{
	Entity *e = getFreeEntity();

	if (e == NULL)
	{
		showErrorAndExit("No free slots to add Tortoise electricity");
	}

	loadProperties("enemy/tortoise_electricity", e);

	playSoundToMap("sound/enemy/tortoise/tortoise_electric.ogg", -1, self->x, self->y, 0);

	setEntityAnimation(e, STAND);

	e->action = &doElectricity;

	e->touch = &entityTouch;

	e->takeDamage = &takeDamage;

	e->draw = &drawLoopingAnimationToMap;

	e->target = self;

	e->face = self->face;

	e->x = self->x;

	e->y = self->y;

	self->target = e;

	self->frameSpeed = 1;

	setEntityAnimation(self, ATTACK_2);

	self->action = &electrify;

	self->thinkTime = 120;
}

static void electrify()
{
	self->thinkTime--;

	self->element = LIGHTNING;

	if (self->thinkTime <= 0)
	{
		self->frameSpeed = -1;

		setEntityAnimation(self, CUSTOM_1);

		self->animationCallback = &electrifyFinish;

		self->action = &entityWait;

		self->element = NO_ELEMENT;

		self->target->inUse = FALSE;
	}

	checkToMap(self);
}

static void electrifyFinish()
{
	setEntityAnimation(self, STAND);

	self->frameSpeed = 1;

	self->action = &walk;

	self->dirX = self->face == LEFT ? -self->speed : self->speed;

	self->thinkTime = 120 + prand() % 120;

	checkToMap(self);
}

static void takeDamage(Entity *other, int damage)
{
	Entity *temp;

	if (damage != 0)
	{
		if (self->element == NO_ELEMENT)
		{
			if ((self->mental == 0 && other->element == LIGHTNING) ||
				(self->mental == 1 && other->element == FIRE) ||
				(self->mental == 2 && other->element == ICE))
			{
				if (self->flags & INVULNERABLE)
				{
					return;
				}

				if (damage != 0)
				{
					self->health += damage;

					if (other->type == PROJECTILE)
					{
						other->target = self;
					}

					setCustomAction(self, &flashWhite, 6, 0, 0);

					/* Don't make an enemy invulnerable from a projectile hit, allows multiple hits */

					if (other->type != PROJECTILE)
					{
						setCustomAction(self, &invulnerableNoFlash, 20, 0, 0);
					}

					if (self->pain != NULL)
					{
						self->pain();
					}

					if (prand() % 5 == 0)
					{
						setInfoBoxMessage(90, 255, 255, 255, _("The damage from this weapon is being absorbed..."));
					}
				}
			}

			else
			{
				entityTakeDamageNoFlinch(other, damage);
			}
		}

		else
		{
			if (other->type == WEAPON)
			{
				/* Damage the player instead */

				temp = self;

				self = other->parent;

				self->takeDamage(temp, temp->damage);

				self = temp;

				return;
			}

			else if (other->type == PROJECTILE)
			{
				self->health -= damage;

				other->target = self;
			}

			if (self->health > 0)
			{
				setCustomAction(self, &flashWhite, 6, 0, 0);
				setCustomAction(self, &invulnerableNoFlash, 20, 0, 0);

				if (self->pain != NULL)
				{
					self->pain();
				}
			}

			else
			{
				self->damage = 0;

				increaseKillCount();

				self->die();
			}
		}

		if (other->type == PROJECTILE)
		{
			temp = self;

			self = other;

			self->die();

			self = temp;
		}
	}
}

static void doElectricity()
{
	if (self->target->health <= 0)
	{
		self->inUse = FALSE;
	}

	self->x = self->target->x + self->target->w / 2 - self->w / 2;
	self->y = self->target->y + self->target->h - self->h;
}

static void breatheFireInit()
{
	Entity *e;

	setEntityAnimation(self, ATTACK_1);

	e = addProjectile("enemy/fireball", self, 0, 0, (self->face == LEFT ? -6 : 6), 0);

	if (self->face == LEFT)
	{
		e->x = self->x + self->w - e->w - self->offsetX;
	}

	else
	{
		e->x = self->x + self->offsetX;
	}

	e->y = self->y + self->offsetY;

	e->flags |= FLY;

	playSoundToMap("sound/enemy/fireball/fireball.ogg", -1, self->x, self->y, 0);

	self->thinkTime = 30;

	self->action = &breatheFireWait;

	checkToMap(self);
}

static void breatheFireWait()
{
	self->thinkTime--;

	if (self->thinkTime <= 0)
	{
		setEntityAnimation(self, JUMP);

		self->thinkTime = 300 + prand() % 180;

		self->dirX = self->face == LEFT ? -self->speed : self->speed;

		self->action = &walk;
	}

	checkToMap(self);
}

static void iceAttackStart()
{
	self->dirX = self->standingOn == NULL ? 0 : self->standingOn->dirX;

	self->frameSpeed = 0;

	self->thinkTime--;

	if (self->thinkTime <= 0)
	{
		self->frameSpeed = 1;

		setEntityAnimation(self, CUSTOM_2);

		self->animationCallback = &createIce;
	}

	checkToMap(self);
}

static void createIce()
{
	Entity *e = getFreeEntity();

	if (e == NULL)
	{
		showErrorAndExit("No free slots to add an Ice Tortoise Ice Ball");
	}

	loadProperties("enemy/ice_tortoise_ice_ball", e);

	setEntityAnimation(e, STAND);

	e->x = self->x + 71;
	e->y = self->y + 21;

	e->dirX = 2;
	e->dirY = -2;

	e->action = &iceBallMove;
	e->draw = &drawLoopingAnimationToMap;
	e->touch = &entityTouch;
	e->fallout = &entityDieNoDrop;

	e = getFreeEntity();

	if (e == NULL)
	{
		showErrorAndExit("No free slots to add an Ice Tortoise Ice Ball");
	}

	loadProperties("enemy/ice_tortoise_ice_ball", e);

	setEntityAnimation(e, STAND);

	e->x = self->x + self->w - e->w - 71;
	e->y = self->y + 21;

	e->dirX = -2;
	e->dirY = -2;

	e->action = &iceBallMove;
	e->draw = &drawLoopingAnimationToMap;
	e->touch = &entityTouch;
	e->fallout = &entityDieNoDrop;

	self->frameSpeed = 1;

	setEntityAnimation(self, CUSTOM_4);

	self->action = &iceAttack;

	self->thinkTime = 120;
}

static void iceAttack()
{
	self->dirX = self->standingOn == NULL ? 0 : self->standingOn->dirX;

	self->thinkTime--;

	if (self->thinkTime <= 0)
	{
		self->frameSpeed = -1;

		setEntityAnimation(self, CUSTOM_2);

		self->animationCallback = &iceAttackFinish;

		self->action = &entityWait;
	}

	checkToMap(self);
}

static void iceAttackFinish()
{
	self->dirX = self->standingOn == NULL ? 0 : self->standingOn->dirX;

	setEntityAnimation(self, WALK);

	self->frameSpeed = 1;

	self->action = &walk;

	self->dirX = self->face == LEFT ? -self->speed : self->speed;

	self->thinkTime = 120 + prand() % 120;

	checkToMap(self);
}

static void iceBallMove()
{
	int x;

	checkToMap(self);

	if (self->flags & ON_GROUND)
	{
		x = mapTileAt(self->x / TILE_SIZE, (self->y + self->h + 5) / TILE_SIZE);

		if (x >= SOLID_TILE_START && x <= SOLID_TILE_END)
		{
			self->layer = MID_GROUND_LAYER;

			x = self->x + self->w / 2;

			setEntityAnimation(self, WALK);

			self->x = x - self->w / 2;

			self->action = &iceFloorWait;

			self->y++;

			self->thinkTime = 30;
		}

		else
		{
			self->inUse = FALSE;
		}
	}

	else if (self->standingOn != NULL)
	{
		self->inUse = FALSE;
	}

	else
	{
		self->health--;

		if (self->health <= 0)
		{
			self->inUse = FALSE;
		}
	}
}

static void iceFloorWait()
{
	Entity *e;

	self->thinkTime--;

	if (self->thinkTime == 0)
	{
		e = getFreeEntity();

		if (e == NULL)
		{
			showErrorAndExit("No free slots to add an Upside Down Ice Spike");
		}

		loadProperties("enemy/ice_spike_upside_down", e);

		setEntityAnimation(e, STAND);

		e->x = self->x;
		e->y = self->y + self->h + 8;

		e->startY = self->y + self->h - e->h;

		e->action = &iceSpikeMove;
		e->touch = &entityTouch;
		e->takeDamage = &spikeTakeDamage;
		e->draw = &drawLoopingAnimationToMap;

		e->head = self;
	}
}

static void iceSpikeMove()
{
	int i;
	Entity *e;

	self->dirX = 0;

	if (self->y > self->startY)
	{
		self->y -= 4;

		if (self->y <= self->startY)
		{
			self->y = self->startY;

			if (self->head != NULL)
			{
				self->head->inUse = FALSE;

				self->head = NULL;
			}
		}
	}

	else
	{
		checkToMap(self);

		self->thinkTime--;

		if (self->thinkTime <= 0)
		{
			self->takeDamage = NULL;

			playSoundToMap("sound/common/shatter.ogg", -1, self->x, self->y, 0);

			for (i=0;i<8;i++)
			{
				e = addTemporaryItem("misc/ice_spike_upside_down_piece", self->x, self->y, RIGHT, 0, 0);

				e->x = self->x + self->w / 2;
				e->x -= e->w / 2;

				e->y = self->y + self->h / 2;
				e->y -= e->h / 2;

				e->dirX = (prand() % 4) * (prand() % 2 == 0 ? -1 : 1);
				e->dirY = ITEM_JUMP_HEIGHT * 2 + (prand() % ITEM_JUMP_HEIGHT);

				setEntityAnimation(e, i);

				e->thinkTime = 60 + (prand() % 60);

				e->touch = NULL;
			}

			self->inUse = FALSE;

			if (self->head != NULL)
			{
				self->head->inUse = FALSE;
			}
		}
	}
}

static void spikeTakeDamage(Entity *other, int damage)
{
	Entity *temp;

	if (self->flags & INVULNERABLE)
	{
		return;
	}

	if (damage != 0)
	{
		self->health -= damage;

		if (other->type == PROJECTILE)
		{
			temp = self;

			self = other;

			self->die();

			self = temp;
		}

		if (self->health > 0)
		{
			setCustomAction(self, &flashWhite, 6, 0, 0);

			/* Don't make an enemy invulnerable from a projectile hit, allows multiple hits */

			if (other->type != PROJECTILE)
			{
				setCustomAction(self, &invulnerableNoFlash, 20, 0, 0);
			}

			enemyPain();
		}

		else
		{
			self->thinkTime = 0;

			self->takeDamage = NULL;
		}
	}
}

static void becomeRampaging()
{
	printf("Rampaging\n");

	self->health = 1;

	self->action = &walk;
}