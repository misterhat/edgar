#include "custom_actions.h"

void setCustomAction(Entity *e, void (*func)(int *), int thinkTime)
{
	int i;

	for (i=0;i<MAX_CUSTOM_ACTIONS;i++)
	{
		if (e->customThinkTime[i] == 0)
		{
			e->custom[i] = func;
			
			e->customThinkTime[i] = thinkTime;
			
			return;
		}
	}
	
	printf("No free slots for Custom Action\n");
	
	exit(1);
}

void pushBack(int *thinkTime)
{
	(*thinkTime)--;
	
	if (*thinkTime != 0)
	{
		self->flags |= HELPLESS;
	}
	
	else
	{
		self->flags &= ~HELPLESS;
	}
}

void invulnerable(int *thinkTime)
{
	(*thinkTime)--;
	
	if (*thinkTime != 0)
	{
		self->flags |= INVULNERABLE;
	}
	
	else
	{
		self->flags &= ~INVULNERABLE;
	}
	
	if ((*thinkTime) % 3 == 0)
	{
		self->flags ^= NO_DRAW;
	}
	
	if ((*thinkTime) == 0)
	{
		self->flags &= ~NO_DRAW;
	}
}