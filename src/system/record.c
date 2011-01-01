/*
Copyright (C) 2009-2011 Parallel Realities

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

#include "random.h"
#include "../graphics/save_png.h"
#include "../map.h"
#include "error.h"

static void saveBuffer(void);
static void loadBuffer(void);

static int frame = 0;
static Input inputBuffer[MAX_INPUTS];
static int bufferID = 0;
static FILE *replayBuffer;
static int inputsRead = 0;
static char screenshotPath[MAX_PATH_LENGTH];

extern Game game;

void setReplayData(char *name, int loadedGame)
{
	char mapFile[6];
	double version;
	long seed;
	int read;

	printf("Setting replay file to %s\n", name);

	replayBuffer = fopen(name, "rb");

	if (replayBuffer == NULL)
	{
		showErrorAndExit("Failed to open replay data %s", name);
	}

	game.gameType = REPLAYING;

	read = fread(&version, sizeof(double), 1, replayBuffer);

	printf("This version : %0.2f Replay version %0.2f\n", VERSION, version);

	if (version != VERSION)
	{
		printf("This replay is from a different version and may not function correctly\n");
	}

	read = fread(&seed, sizeof(long), 1, replayBuffer);

	printf("Setting seed %ld\n", seed);

	setSeed(seed);

	read = fread(mapFile, 5, 1, replayBuffer);

	mapFile[5] = '\0';

	if (loadedGame == FALSE)
	{
		printf("Loading map %s\n", mapFile);

		loadMap(mapFile, TRUE);
	}
}

void setRecordData(char *name)
{
	long seed;
	double version = VERSION;

	printf("Setting record file to %s\n", name);

	replayBuffer = fopen(name, "wb");

	if (replayBuffer == NULL)
	{
		showErrorAndExit("Failed to open replay data %s", name);
	}

	game.gameType = RECORDING;

	seed = time(NULL);

	fwrite(&version, sizeof(double), 1, replayBuffer);

	fwrite(&seed, sizeof(long), 1, replayBuffer);

	printf("Setting seed %ld\n", seed);

	setSeed(seed);
}

void setMapFile(char *name)
{
	printf("Setting map to %s\n", name);

	fwrite(name, 5, 1, replayBuffer);
}

void setScreenshotDir(char *name)
{
	STRNCPY(screenshotPath, name, sizeof(screenshotPath));

	printf("Set screenshot directory to %s\n", screenshotPath);
}

void takeScreenshot()
{
	char filename[MAX_PATH_LENGTH];

	if (strlen(screenshotPath) != 0)
	{
		snprintf(filename, sizeof(filename), "%s/edgar%06d.bmp", screenshotPath, frame);

		frame++;

		SDL_SaveBMP(game.screen, filename);
	}
}

void takeSingleScreenshot()
{
	SDL_SaveBMP(game.screen, "edgar.bmp");
}

void putBuffer(Input inp)
{
	inputBuffer[bufferID] = inp;

	bufferID++;

	if (bufferID == MAX_INPUTS)
	{
		saveBuffer();

		bufferID = 0;
	}
}

Input getBuffer()
{
	Input inp;

	if (bufferID == 0)
	{
		loadBuffer();
	}

	if (inputsRead != MAX_INPUTS && bufferID == inputsRead)
	{
		printf("End of replay\n");

		exit(0);
	}

	inp = inputBuffer[bufferID];

	bufferID++;

	if (bufferID == MAX_INPUTS)
	{
		bufferID = 0;
	}

	return inp;
}

static void saveBuffer()
{
	fwrite(inputBuffer, sizeof(Input), bufferID, replayBuffer);
}

static void loadBuffer()
{
	inputsRead = fread(inputBuffer, sizeof(Input), MAX_INPUTS, replayBuffer);

	if (inputsRead != MAX_INPUTS)
	{
		printf("Replay buffer not completely filled. Only read %d\n", inputsRead);
	}
}

void flushBuffer(int gameType)
{
	if (gameType == RECORDING)
	{
		saveBuffer();
	}

	if (replayBuffer != NULL)
	{
		fclose(replayBuffer);
	}
}
