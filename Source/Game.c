#include "Game.h"		// Header for Game constants and accessors.
#include "Sounds.h"		// For jump sound effect.

#define MOVESTEP (1.0f/8.0f)		// Move by 1/8th of a block to support smooth animation.

extern bool enableBarrels;			// Make enableBarrels global so it can be used in the game.
									// This is a bit of a fiddle.

// The x and y parameters are floating point numbers so that they can be moved by a fraction of a whole game table block for smooth sprite movement.
// The x and y are scaled up the main program for the size of sprites used and the size of the graphics screen.
typedef struct
{
	float x;	// x inside the game store max GAMEX
	float y;	// y inside the game store max GAMEY
	float dir;	// direction step (negative for left and positive for right)
	char baddy;	// type of baddy.
}  BaddyT;

// There is only one Mario that has a bigger range of movement, so has a different structure.
typedef struct
{
	float x;	// x inside the game store max GAMEX
	float y;	// y inside the game store max GAMEY
	char face;	// direction 'L' Left, 'R' Right, 'U' Up/Down, ' ' Not moving
	bool jump;	// true indicates jumping.
}  MarioT;

// The arrays for the game screens for the 4 levels of the game.
static char scr1[GAMEY][GAMEX] = {
	   { EG, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, DK, DI, SP, SP, SP, SP, SP, QM, Q2, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, DI, DI, SP, SP, SP, SP, SP, Q2, Q2, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, BR, BR, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, BR, BR, PH, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, GD, GD, GD, GD, GD, GD, L2, GD, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, SP, SP, SP, SP, SP, SP, LD, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, SP, SP, SP, SP, SP, SP, LD, SP, SP, GD, GD, GD, GD, GD, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, SP, SP, SP, SP, SP, SP, LD, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, SP, SP, SP, SP, SP, SP, GD, GD, GD, GD, GD, GD, GD, L2, GD, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, SP, SP, GD, GD, L2, SP, SP, SP, SP, SP, SP, SP, SP, LD, SP, SP, SP, GD, GD, GD, L2, SP, SP, SP, EG },
	   { EG, SP, SP, SP, SP, LD, SP, SP, SP, GD, L2, GD, GD, GD, GD, SP, SP, SP, SP, SP, SP, LD, SP, SP, SP, EG },
	   { EG, SP, GD, GD, GD, GD, GD, SP, SP, SP, LD, SP, SP, SP, SP, SP, GD, L2, GD, GD, GD, GD, GD, GD, SP, EG },
	   { EG, SP, SP, SP, SP, SP, SP, SP, SP, SP, LD, SP, SP, SP, SP, SP, SP, LD, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, SP, SP, SP, SP, SP, SP, SP, GD, GD, GD, GD, GD, GD, GD, GD, SP, LD, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, EG } };

static char scr2[GAMEY][GAMEX] = {
	   { EG, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, SP, DK, DI, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, SP, DI, DI, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, SP, BR, BR, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, SP, BR, BR, PH, SP, SP, SP, SP, SP, SP, SP, QM, Q2, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, SP, GD, GD, GD, GD, GD, GD, SP, SP, SP, SP, Q2, Q2, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, SP, SP, SP, SP, SP, GD, GD, L2, SP, SP, SP, SP, SP, GD, GD, L2, GD, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, SP, SP, SP, SP, SP, SP, SP, LD, SP, SP, SP, SP, SP, SP, SP, LD, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, SP, SP, SP, SP, SP, SP, SP, LD, SP, SP, GD, L2, GD, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, SP, SP, SP, SP, SP, SP, SP, LD, SP, SP, SP, LD, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, SP, SP, SP, SP, SP, L2, GD, GD, SP, SP, SP, LD, SP, SP, GD, GD, GD, GD, L2, SP, SP, SP, SP, SP, EG },
	   { EG, SP, SP, SP, SP, SP, LD, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, LD, SP, SP, SP, SP, SP, EG },
	   { EG, SP, SP, SP, SP, SP, LD, SP, SP, SP, L2, GD, GD, GD, GD, GD, GD, GD, SP, LD, SP, SP, SP, SP, SP, EG },
	   { EG, SP, SP, SP, SP, SP, SP, SP, SP, SP, LD, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, SP, SP, SP, SP, GD, GD, GD, GD, GD, GD, SP, SP, SP, SP, SP, SP, L2, GD, GD, GD, GD, SP, SP, SP, EG },
	   { EG, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, LD, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, EG } };

static char scr3[GAMEY][GAMEX] = {
	   { EG, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, QM, Q2, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, Q2, Q2, SP, SP, SP, L2, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, L2, SP, SP, SP, SP, SP, EG },
	   { EG, SP, SP, SP, SP, SP, LD, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, LD, SP, SP, SP, SP, SP, EG },
	   { EG, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, BR, DK, DI, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, SP, GD, GD, GD, GD, GD, GD, GD, SP, SP, BR, DI, DI, PH, SP, SP, GD, GD, L2, SP, SP, SP, SP, SP, EG },
	   { EG, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, GD, GD, GD, GD, SP, SP, SP, SP, LD, SP, SP, SP, SP, SP, EG },
	   { EG, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, SP, SP, SP, SP, SP, GD, GD, GD, GD, SP, SP, SP, SP, SP, SP, GD, GD, GD, GD, SP, SP, SP, SP, SP, EG },
	   { EG, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, GD, GD, GD, GD, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, GD, GD, GD, GD, EG },
	   { EG, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, GD, GD, GD, GD, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, SP, SP, SP, SP, SP, GD, GD, GD, SP, SP, SP, SP, SP, SP, SP, SP, GD, GD, GD, SP, SP, SP, SP, SP, EG },
	   { EG, GD, GD, GD, SP, SP, SP, SP, FL, DI, SP, SP, SP, SP, SP, SP, FL, DI, SP, SP, SP, SP, GD, GD, GD, EG },
	   { EG, SP, SP, SP, SP, SP, SP, SP, DI, DI, SP, GD, GD, GD, GD, SP, DI, DI, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, SP, SP, SP, SP, GD, L2, GD, GD, GD, FL, DI, FL, DI, FL, DI, GD, GD, GD, L2, GD, SP, SP, SP, SP, EG },
	   { EG, SP, SP, SP, SP, SP, LD, SP, SP, SP, DI, DI, DI, DI, DI, DI, SP, SP, SP, LD, SP, SP, SP, SP, SP, EG },
	   { EG, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, GD, EG } };

static char scr4[GAMEY + 1][GAMEX] = {	// Allow an extra edge at the bottom as a catch all in case a barrel falls through.
	   { EG, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, QM, Q2, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, Q2, Q2, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, PH, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, SP, SP, SP, SP, SP, SP, SP, SP, SP, L2, GD, GD, GD, GD, L2, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, SP, SP, SP, SP, SP, SP, SP, SP, SP, LD, BR, DK, DI, SP, LD, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, SP, SP, SP, SP, SP, SP, SP, SP, SP, LD, BR, DI, DI, SP, LD, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, SP, SP, SP, L2, GD, GD, GD, GD, SP, GD, GD, GD, GD, GD, GD, GD, SP, GD, GD, GD, L2, SP, SP, SP, EG },
	   { EG, SP, SP, SP, LD, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, LD, SP, SP, SP, EG },
	   { EG, SP, SP, SP, LD, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, LD, SP, SP, SP, EG },
	   { EG, SP, SP, SP, LD, SP, SP, PI, DI, SP, SP, SP, PI, DI, SP, SP, SP, PI, DI, SP, SP, LD, SP, SP, SP, EG },
	   { EG, SP, SP, GD, GD, GD, GD, GD, GD, GD, GD, L2, GD, GD, L2, GD, GD, GD, GD, GD, GD, GD, GD, SP, SP, EG },
	   { EG, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, LD, SP, SP, LD, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, LD, SP, SP, LD, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, LD, SP, SP, LD, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, EG },
	   { EG, SP, GD, GD, GD, GD, GD, GD, GD, SP, GD, GD, GD, GD, GD, GD, SP, GD, GD, GD, GD, GD, GD, L2, SP, EG },
	   { EG, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, LD, SP, EG },
	   { EG, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP, LD, SP, EG },
	   { EG, SP, SP, SP, SP, SP, FL, DI, SP, SP, SP, SP, FL, DI, SP, SP, SP, SP, FL, DI, SP, SP, SP, LD, SP, EG },
	   { EG, GD, GD, GD, GD, GD, DI, DI, GD, GD, GD, GD, DI, DI, GD, GD, GD, GD, DI, DI, GD, GD, GD, GD, GD, EG },
	   { EG, EG, EG, EG, EG, EG, EG, EG, EG, EG, EG, EG, EG, EG, EG, EG, EG, EG, EG, EG, EG, EG, EG, EG, EG, EG } }; // Extra set of edge to stop risk of barrels falling through screen.

// Table of jump y values to give jump the correct shape for some gravity. The table goes up for 18 steps,
// then down for 18 steps, with a couple of 8s on the end to keep moving down if a long drop.
// Remember y counts down from the top so negative is going up.
// Also remember that movements are fractions of a block. In this case use the 32 pixel block size is used to work out the fractions.
// This may need rework if the block size ever changes.
static float jmpTable[38] = { -7.0f / 32.0f, -7.0f / 32.0f, -6.0f / 32.0f, -6.0f / 32.0f, -5.0f / 32.0f, -5.0f / 32.0f, -4.0f / 32.0f, -4.0f / 32.0f,
							  -4.0f / 32.0f, -4.0f / 32.0f, -3.0f / 32.0f, -3.0f / 32.0f, -2.0f / 32.0f, -2.0f / 32.0f, -1.0f / 32.0f, -1.0f / 32.0f, -1.0f / 32.0f, -1.0f / 32.0f,
							   1.0f / 32.0f,  1.0f / 32.0f,  1.0f / 32.0f,  1.0f / 32.0f,  2.0f / 32.0f,  2.0f / 32.0f,  3.0f / 32.0f,  3.0f / 32.0f,  4.0f / 32.0f,  4.0f / 32.0f,
							   4.0f / 32.0f,  4.0f / 32.0f,  5.0f / 32.0f,  5.0f / 32.0f,  6.0f / 32.0f,  6.0f / 32.0f,  7.0f / 32.0f,  7.0f / 32.0f,
							   8.0f / 32.0f,  8.0f / 32.0f };

static char store[GAMEY][GAMEX];	// Working store for the current screen.
static BaddyT baddies[MAXBADDY];	// Array for all the baddies control.
static BaddyT falling[MAXBADDY];	// Array for falling barrels dropped by DK.
static MarioT Mario;				// Mario control data.
static unsigned int MarioState = 2;	// Mario moving state 0 jumping, 1 falling, 2 control moving. Start ready to accept controls.
static unsigned int jmpCnt;			// Count to control jump movement.
static unsigned int gameLevel = 0;	// Local copy of the game level passed in (used for level specific behaviour).

// Integer conversion just chops the decimal part from the number.
// This function adds 0.5 so that the rounding is to the closest integer, not the lowest.
static unsigned int toInt(float a)
{
	return ((unsigned int)(a + 0.5f));
}

// Function to tell if a floating point has a decimal part (to 1/1000th resolution).
static bool isNotInt(float a)
{
	float b, c;

	b = (float)((int)(a));
	c = b - a;
	if ((c > 0.0001) || (c < -0.0001))
	{
		return true;
	}
	return false;
}

// Initialise the game level.
void initGame(unsigned int level)
{
	// Copy the constant game table for the level requested to the game store.
	for (unsigned int y = 0; y < GAMEY; y++)
	{
		for (unsigned int x = 0; x < GAMEX; x++)
		{
			// The default is the first level of the game.
			if (level == 2)			{ store[y][x] = scr2[y][x]; }
			else if (level == 3)	{ store[y][x] = scr3[y][x]; }
			else if (level == 4)	{ store[y][x] = scr4[y][x]; }
			else					{ store[y][x] = scr1[y][x]; }
		}
	}

	// Keep a copy of the level for any level specific game behaviour.
	gameLevel = level;

	// Clear out any baddies from previous level (or uninitialised).
	for (unsigned int a = 0; a < MAXBADDY; a++)
	{
		baddies[a].x = 0.0f;
		baddies[a].y = 0.0f;
		baddies[a].baddy = 0;	
		baddies[a].dir = 0.0f;
	}

	// Clear out any falling barrels from previous level (or uninitialised).
	for (unsigned int a = 0; a < MAXBADDY; a++)
	{
		falling[a].x = 0.0f;
		falling[a].y = 0.0f;
		falling[a].baddy = 0;
		falling[a].dir = 0.0f;
	}

	// Initialise the specific baddies for the level.
	if (level == 1)
	{
		baddies[0].x = 20.0f;
		baddies[0].y = 18.0f;
		baddies[0].baddy = GB;	// Goomba
		baddies[0].dir = MOVESTEP;
		baddies[1].x = 14.0f;
		baddies[1].y = 11.0f;
		baddies[1].baddy = GB;
		baddies[1].dir = MOVESTEP;
	}
	else if (level == 2)
	{
		baddies[0].x = 20.0f;
		baddies[0].y = 18.0f;
		baddies[0].baddy = GB;
		baddies[0].dir = MOVESTEP;
		baddies[1].x = 14.0f;
		baddies[1].y = 14.0f;
		baddies[1].baddy = GB;
		baddies[1].dir = MOVESTEP;
		baddies[2].x = 10.0f;
		baddies[2].y = 18.0f;
		baddies[2].baddy = GB;
		baddies[2].dir = MOVESTEP;
	}
	else if (level == 3)
	{
		baddies[0].x =  9.0f;
		baddies[0].y = 18.0f;
		baddies[0].baddy = GB;
		baddies[0].dir = MOVESTEP;
		baddies[1].x = 14.0f;
		baddies[1].y =  2.0f;
		baddies[1].baddy = GB;
		baddies[1].dir = MOVESTEP;
		baddies[2].x =  8.0f;
		baddies[2].y =  5.0f;
		baddies[2].baddy = GB;
		baddies[2].dir = MOVESTEP;
		baddies[3].x = 20.0f;
		baddies[3].y = 18.0f;
		baddies[3].baddy = GB;
		baddies[3].dir = MOVESTEP;
		baddies[4].x = 14.0f;
		baddies[4].y = 14.0f;
		baddies[4].baddy = GB;
		baddies[4].dir = MOVESTEP;
	}
	else // Do level 4 as the default
	{
		baddies[0].x =  4.0f;
		baddies[0].y =  6.0f;
		baddies[0].baddy = GB;
		baddies[0].dir = MOVESTEP;
		baddies[1].x = 21.0f;
		baddies[1].y =  6.0f;
		baddies[1].baddy = GB;
		baddies[1].dir = MOVESTEP;
		baddies[2].x =  2.0f;
		baddies[2].y = 14.0f;
		baddies[2].baddy = GB;
		baddies[2].dir = MOVESTEP;
		baddies[3].x = 23.0f;
		baddies[3].y = 14.0f;
		baddies[3].baddy = GB;
		baddies[3].dir = MOVESTEP;
		baddies[4].x = 20.0f;
		baddies[4].y = 10.0f;
		baddies[4].baddy = GB;
		baddies[4].dir = MOVESTEP;
		baddies[5].x = 14.0f;
		baddies[5].y = 14.0f;
		baddies[5].baddy = GB;
		baddies[5].dir = MOVESTEP;
	}

	// Put Mario in the starting position.
	Mario.face = ' ';
	Mario.jump = false;
	Mario.x = 2.0f;
	Mario.y = 18.0f;
}

// Move falling barrels
void moveFallingBarrels(void)
{
	static unsigned int cnt = 50;

	cnt++;	// Done inside the loop so all barrels aren't done at once.

	// Go through the falling barrels (the number is set by the level).
	for (int a = 0; a < MAXBADDY; a++)
	{
		// If it isn't a barrel, set it to a barrel.
		// cnt is used to leave a reasonable gap between barrels.
		// gameLevel is used to make the barrels come more quickly
		if ((falling[a].baddy != BL) && ((cnt % (120 - (gameLevel * 5))) == 0))
		{
			cnt = 1;	// Set back to zero ready for the next barrel.
			// Set the barrel to start at Donkey Kong (which is a bit different for each level.
			falling[a].baddy = BL;
			if      (gameLevel == 1) { falling[a].x =  2.0f; falling[a].y = 4.0f; }
			else if (gameLevel == 2) { falling[a].x =  3.0f; falling[a].y = 4.0f; }
			else if (gameLevel == 3) { falling[a].x = 12.0f; falling[a].y = 6.0f; }
			else if (gameLevel == 4) { falling[a].x = 12.0f; falling[a].y = 6.0f; }

			// Set the barrel to roll towards Mario when it lands.
			if (Mario.x < falling[a].x) { falling[a].dir = (MOVESTEP * -2.0f); }
			else { falling[a].dir = (MOVESTEP * 2.0f); }
		}
		else if (falling[a].baddy == BL)
		{
			// If the barrel has not landed fall straight down.
			if ((store[toInt(falling[a].y + 0.5f)][toInt(falling[a].x)] != GD) && (store[toInt(falling[a].y + 0.5)][toInt(falling[a].x)] != L2))
			{
				// Fall faster than sideways movement.
				falling[a].y = falling[a].y + (MOVESTEP * 2.0f);

				// Set the barrel to roll towards Mario when it lands.
				if (Mario.x < falling[a].x) { falling[a].dir = (MOVESTEP * -2.0f); }
				else { falling[a].dir = (MOVESTEP * 2.0f); }

				// An extra trap in case the barrel has fallen through to the bottom of the last screen.
				if (store[toInt(falling[a].y)][toInt(falling[a].x)] == EG)
				{
					// Clear everything out, so that it can start again.
					falling[a].x = 0.0f;
					falling[a].y = 0.0f;
					falling[a].dir = 0.0f;
					falling[a].baddy = ' ';
				}
			}
			else
			{
				// If on the ground move in the chosen direction.
				falling[a].x = falling[a].x + falling[a].dir;
				// When the barrel has reached the edge make it disappear.
				if (store[toInt(falling[a].y)][toInt(falling[a].x)] == EG)
				{
					// Clear everything out, so that it can start again.
					falling[a].x = 0.0f;
					falling[a].y = 0.0f;
					falling[a].dir = 0.0f;
					falling[a].baddy = ' ';
				}
			}
		}
	}
}

// Keep the baddies moving back and forth.
void moveBaddies(void)
{
	// Only once falling barrels are enabled do the processing.
	if (enableBarrels == true) 
	{
		// Call the function to move the falling barrels as well.
		moveFallingBarrels();
	}

	// Move all baddies
	for (unsigned int a = 0; a < MAXBADDY; a++)
	{
		// Only move a baddy if it has been initialised.
		if (baddies[a].baddy != 0)
		{
			// If the baddy is moving right, check if it can still keep moving right.
			if (baddies[a].dir > 0.0)
			{
				// Remember y increases down the screen. Keep moving in the same direction, as long as there girders beneath the baddy.
				if ((store[toInt(baddies[a].y + 1.0f)][toInt(baddies[a].x)] == GD) || (store[toInt(baddies[a].y + 1.0f)][toInt(baddies[a].x)] == L2))
				{
					// Move right
					baddies[a].x = baddies[a].x + baddies[a].dir;
				}
				else
				{
					// Change direction to left.
					baddies[a].dir = -MOVESTEP;
					// Move a bit left as well so don't get stuck.
					baddies[a].x = baddies[a].x + baddies[a].dir;
				}
			}
			// If baddy is moving left check if it can keep going.
			else
			{
				// Remember y increases down the screen. Keep moving in the same direction, as long as there girders beneath the baddy.
				if ((store[toInt(baddies[a].y + 1.0f)][toInt(baddies[a].x)] == GD) || (store[toInt(baddies[a].y + 1.0f)][toInt(baddies[a].x)] == L2))
				{
					// Move left.
					baddies[a].x = baddies[a].x + baddies[a].dir;
				}
				else
				{
					// Change direction to right.
					baddies[a].dir = MOVESTEP;
					// Move a bit right as well so don't get stuck.
					baddies[a].x = baddies[a].x + baddies[a].dir;
				}
			}
		}
	}
}

// Do logic for Mario Jumping.
char jumpMario()
{
	// If it is the start of the jump, play the sound effect.
	if (jmpCnt == 0)
	{
		putsoundSel(JUMP);
	}

	// If moving right then jump right.
	if (Mario.face == 'R')
	{
		Mario.x = Mario.x + MOVESTEP;
	}
	// If moving left then jump left.
	else if (Mario.face == 'L')
	{
		Mario.x = Mario.x - MOVESTEP;
	}
	// Otherwise the jump will be straight up.
	Mario.y = Mario.y + jmpTable[jmpCnt];			// Table to shape Mario jump
	// If jumping straight up, face up.
	if (Mario.face == ' ') { Mario.face = 'U'; }

	if ((store[toInt(Mario.y)][toInt(Mario.x)] == QM) || (store[toInt(Mario.y)][toInt(Mario.x)] == Q2))
	{
		MarioState = 1;		// Go back to falling state
		Mario.jump = false;	// and stop jump.
		return 'E';			// return that this is the end of the level.
	}
	// If the jump has hit the edge, stop jumping and start falling.
	else if (store[toInt(Mario.y)][toInt(Mario.x)] == EG)
	{
		MarioState = 1;		// Go back to falling state
		Mario.jump = false;	// and stop jump.

		// Come back in to the column next to the edge.
		if (Mario.face == 'R')
		{
			Mario.x = (float)(toInt(8.0f * (Mario.x - MOVESTEP))) / 8.0f;	// Come back in from the edge a little bit and get back onto 1/8 of a block.
			Mario.y = (float)(toInt(Mario.y));			// Bring Mario.y back onto an integer so that it will fall cleanly and end.
		}
		// If moving left then jump left.
		else if (Mario.face == 'L')
		{
			Mario.x = (float)(toInt(8.0f * (Mario.x + MOVESTEP))) / 8.0f;	// Come back in from the edge a little bit and get back onto 1/8 of a block.
			Mario.y = (float)(toInt(Mario.y));			// Bring Mario.y back onto an integer so that it will fall cleanly and end.
		}
	}
	// If Mario has landed on a ladder stop the jump.
	// Do ladder processing before landing on top of girder, to avoid some interaction between the two.
	else if ((store[toInt(Mario.y)][toInt(Mario.x)] == LD) || (store[toInt(Mario.y)][toInt(Mario.x)] == L2))
	{
		MarioState = 2;	// Have landed on a ladder go back to control state.
		Mario.jump = false;

		Mario.x = (float)(toInt(Mario.x));	// Bring Mario.x onto a round number to be properly on the ladder.
		Mario.y = (float)(toInt(8.0f * (Mario.y - MOVESTEP))) / 8.0f;	// Bring Mario.y back onto 1/8 steps and up a 1/8th to avoid falling.
	}
	// If Mario is coming down (past middle of jmpTable) and has landed on a girder stop the jump.
	else if ((jmpCnt > 18) && ((store[toInt(Mario.y + 0.5f)][toInt(Mario.x)] == L2) ||	// Top of ladder.
		(store[toInt(Mario.y + 0.5f)][toInt(Mario.x)] == GD)))	// Girder.
	{
		MarioState = 2;	// Have landed on a girder so go back to move control state.
		Mario.jump = false;

		Mario.y = (float)(toInt(Mario.y));	// Bring Mario.y back onto a whole integer.
	}

	// Avoid going off the end of the table if falling further.
	jmpCnt++;
	if (jmpCnt > 36) { jmpCnt = 36; }
	return ' ';
}

// Do logic to control Mario while he is falling.
void fallMario()
{
	// If Mario is not standing on a girder or ladder, then let him fall until he lands on something.
	// Keep him falling until he reaches the bottom of the block (as shown by being a whole integer).
	// Also need to check on whether Mario has got to solid ground (0.5 is needed due to rounding).
	if (((store[toInt(Mario.y + 1.0f)][toInt(Mario.x)] != GD) &&	// On a girder
		 (store[toInt(Mario.y + 1.0f)][toInt(Mario.x)] != L2) &&	// On the top of a ladder
		 (store[toInt(Mario.y)][toInt(Mario.x)] != LD) &&			// Standing on a ladder
		 (store[toInt(Mario.y)][toInt(Mario.x)] != L2))				// or ladder top.
		||
		((isNotInt(Mario.y) == true) &&							// Part way up
			(store[toInt(Mario.y)][toInt(Mario.x)] != LD) &&	// on Ladder.
			(store[toInt(Mario.y)][toInt(Mario.x)] != L2) &&	// on Ladder.
			(store[toInt(Mario.y + 0.5f)][toInt(Mario.x)] != L2) &&	// above Top of ladder.
			(store[toInt(Mario.y + 0.5f)][toInt(Mario.x)] != GD)))	// above Girder.
	{
		MarioState = 1;	// Set state to Falling.

		Mario.y = Mario.y + MOVESTEP + MOVESTEP;	// Move Mario down fast as this looks best.

		// Error trap to ensure that Mario can't keep falling through memory if something has gone wrong (should not be needed).
		if (Mario.y > (GAMEY - 1.0f))
		{
			Mario.y = GAMEY - 1.0f;
		}
		// Leave Mario.face as it was set so Mario keeps facing the same way he was going.
	}
	else
	{
		MarioState = 2;	// Control moving.
	}
}

// Control Mario movement
void controlMario(char move)
{
	Mario.face = ' ';	// Default to standing still.

	// Move left if not yet reached the edge of the screen (as indicated by EG).
	// Also check that not trying to walk sideways through a girder and 
	// that the number is an integer i.e. we are not part way up a block.
	// Also don't allow sideways movement at end of ladder L2.
	// Need to allow sideways movement on ladders to allow mario to walk past ladders.
	// But not if the ladder is below Mario, this means he is part way up a ladder.
	// If there is a space below Mario he is hanging at the bottom of a ladder.
	if ((move == 'L') && (store[toInt(Mario.y)][toInt(Mario.x - MOVESTEP)] != EG)
					  && (store[toInt(Mario.y)][toInt(Mario.x - MOVESTEP)] != GD) 
					  && (store[toInt(Mario.y)][toInt(Mario.x)] != L2) 
					  && (store[toInt(Mario.y + 1.0)][toInt(Mario.x)] != LD)
					  && (store[toInt(Mario.y + 1.0)][toInt(Mario.x)] != SP)
					  && (isNotInt(Mario.y) == false))
	{
		Mario.x = Mario.x - MOVESTEP;

		// Error trap to stop moving at the left of the array, should never be needed.
		if (Mario.x < 0.0f)
		{
			Mario.x = 0.0f;
		}

		// Face left so main program will animate left walking.
		Mario.face = 'L';
	}
	// Move right if not yet reached the edge of the screen.
	// Also check that not trying to walk sideways through a girder and
	// that the number is an integer i.e. we are not part way up a block. 
	// Also don't allow sideways movement at end of ladder L2.
	// Need to allow sideways movement on ladders to allow mario to walk past ladders.
	// But not if the ladder is below Mario, this means he is part way up a ladder.
	// If there is a space below Mario he is hanging at the bottom of a ladder.
	else if ((move == 'R') && (store[toInt(Mario.y)][toInt(Mario.x + MOVESTEP)] != EG)
						   && (store[toInt(Mario.y)][toInt(Mario.x - MOVESTEP)] != GD) 
						   && (store[toInt(Mario.y)][toInt(Mario.x)] != L2)
						   && (store[toInt(Mario.y + 1.0)][toInt(Mario.x)] != LD)
						   && (store[toInt(Mario.y + 1.0)][toInt(Mario.x)] != SP)
						   && (isNotInt(Mario.y) == false))
	{
		Mario.x = Mario.x + MOVESTEP;

		// Error trap at end of array should never be needed.
		if (Mario.x > (GAMEX - 1.0f))
		{
			Mario.x = GAMEX - 1.0f;
		}

		// Show animate walking right.
		Mario.face = 'R';
	}
	// Move Up as long as Mario is still on a ladder.
	else if ((move == 'U') && ((store[toInt(Mario.y - MOVESTEP)][toInt(Mario.x)] == LD) || (store[toInt(Mario.y - MOVESTEP)][toInt(Mario.x)] == L2) || (store[toInt(Mario.y - MOVESTEP + 0.5f)][toInt(Mario.x)] == L2)))
	{
		Mario.x = (float)(toInt(Mario.x));	// Jump to the whole integer so cleanly on the ladder if moving up.
		Mario.y = Mario.y - MOVESTEP;

		// Error trap at end of array should never be needed.
		if (Mario.y < 0.0f)
		{
			Mario.y = 0.0f;
		}

		// Animate going up.
		Mario.face = 'U';
	}
	// Move Down if not on a girder (fall will take over at the bottom of a ladder.
	else if ((move == 'D') && (store[toInt(Mario.y + 0.5f)][toInt(Mario.x)] != GD))
	{
		Mario.x = (float)(toInt(Mario.x));	// Jump to the whole integer so cleanly on ladder if moving down.
		// As long as we haven't got down to a girder keep moving down.
		// Rounding means need to look half a block further to avoid going down into the girder below.
		if (store[toInt(Mario.y + 0.5f)][toInt(Mario.x)] != GD)
		{
			Mario.y = Mario.y + MOVESTEP;
		}

		// Error trap at end of array should never be needed.
		if (Mario.y > (GAMEY - 1.0f))
		{
			Mario.y = GAMEY - 1.0f;
		}

		// Going up and down look the same so return 'U' to animate the same as going up.
		Mario.face = 'U';
	}
}

// Check if Mario has hit any static elements that would kill him (e.g. fire) or any moving baddies.
bool checkMario()
{
	float xdiff, ydiff;	// Difference between baddy position and Mario.

	// Go through Mario current position to check isn't on a hazard in the background.
	if ((store[toInt(Mario.y)][toInt(Mario.x)] == DI) ||
		(store[toInt(Mario.y)][toInt(Mario.x)] == DK) ||
		(store[toInt(Mario.y)][toInt(Mario.x)] == BR) ||
		(store[toInt(Mario.y)][toInt(Mario.x)] == FL) ||
		(store[toInt(Mario.y)][toInt(Mario.x)] == PI))
	{
		return true;
	}

	// Go though all baddies to see if they are touching Mario.
	for (unsigned int a = 0; a < MAXBADDY; a++)
	{
		// Only check a baddy if it has been initialised.
		if (baddies[a].baddy != 0)
		{
			xdiff = baddies[a].x - Mario.x;	// how far Mario is from baddy on x axis.
			ydiff = baddies[a].y - Mario.y;	// how far Mario is from baddy on y axis.

			if (xdiff < 0.0) { xdiff = xdiff * -1.0f; }	// if differences are negative change sign.
			if (ydiff < 0.0) { ydiff = ydiff * -1.0f; }

			// Report if Mario has touched a baddy.
			if ((xdiff <= 0.5f) && (ydiff <= 0.5f))
			{
				return true;
			}
		}
	}

	// Go though all falling barrels to see if they are touching Mario.
	for (unsigned int a = 0; a < MAXBADDY; a++)
	{
		// Only check a baddy if it has been initialised.
		if (falling[a].baddy != 0)
		{
			xdiff = falling[a].x - Mario.x;	// how far Mario is from barrel on x axis.
			ydiff = falling[a].y - Mario.y;	// how far Mario is from barrel on y axis.

			if (xdiff < 0.0) { xdiff = xdiff * -1.0f; }	// if differences are negative change sign.
			if (ydiff < 0.0) { ydiff = ydiff * -1.0f; }

			// Report if Mario has touched a barrel.
			if ((xdiff <= 0.5f) && (ydiff <= 0.5f))
			{
				return true;
			}
		}
	}
	// Mario not died.
	return false;
}

// Move Mario, taking into account jumping or falling.
// If not currently mid jump or falling act on the user controls.
// Move 'U' up, 'D' down, 'L' left, 'R' right. Or start a jump if jump button pressed.
// Returns 'D' for dead, 'E' for end of level or ' '.
char moveMario(char move, bool jump)
{
	char ret = ' ';

	// Check if Mario has died (run into anything that will finish him off).
	if (checkMario() == true) 
	{ 
		return('D'); // Return that Mario has died.
	}

	// If jumping, sort out jumping.
	if (MarioState == 0)
	{
		ret = jumpMario();
	}
	// Otherwise check Mario falling and do movement if not falling.
	else
	{
		// Mario falling function is called all the time if not jumping to detect if Mario is falling.
		fallMario();
		// If Mario is not falling then do movement control.
		if (MarioState == 2)
		{
			// If jump has been selected then go to jumping state.
			if (jump == true)
			{
				MarioState = 0;		// Jumping state
				jmpCnt = 0;			// Jump count back to the start.
				Mario.jump = true;	// Show Mario jumping for animation.
			}
			else  // Control Mario movement.
			{
				controlMario(move);
			}
		}
	}
	return ret;	// status.
}

// Get the block at a position in the game table (for display).
char getGame(unsigned int x, unsigned int y)
{
	return store[y][x];
}

// Pass back the current x and y positions along with the baddy type for the baddy selected and direction.
// If the baddy is not used in this level baddy, xb and yb will all be set to 0.
// Likewise if the baddy selected is outside the range MAXBADDY the values returned will be 0.
void getBaddy(unsigned int idx, float *xb, float *yb, char *baddy, float *dir)
{
	// If the baddy requested is outside the max number of baddies return an empty baddy.
	if (idx >= MAXBADDY)
	{
		*xb = 0.0f;
		*yb = 0.0f;
		*baddy = 0u;
		*dir = 0.0f;
	}
	// If the baddy selected is not set up as a Goomba or Barrel, return an empty baddy
	// This is belt and braces. Unused baddies are set to empty anyway, but it is better to have this trap than rely on initialisation.
	else if ((baddies[idx].baddy != GB) && (baddies[idx].baddy != BL))
	{
		*xb = 0.0f;
		*yb = 0.0f;
		*baddy = 0u;
		*dir = 0.0f;
	}
	// Otherwise return the baddy details.
	else
	{
		*xb = baddies[idx].x;
		*yb = baddies[idx].y;
		*baddy = baddies[idx].baddy;
		*dir = baddies[idx].dir;
	}
}

// Pass back the current x and y positions along with the direction of the falling barrel.
// If the barrel is not used in this level xb and yb will all be set to 0.
// Likewise if the barrel selected is outside the range MAXBADDY the values returned will be 0.
void getFallingBarrel(unsigned int idx, float* xb, float* yb, float* dir)
{
	// If the barrel requested is outside the max number of baddies return an empty barrel.
	if (idx >= MAXBADDY)
	{
		*xb = 0.0f;
		*yb = 0.0f;
		*dir = 0.0f;
	}
	// If the barrel selected is not set up a Barrel, return an empty barrel
	// This is belt and braces. Unused falling barrels are set to empty anyway, but it is better to have this trap than rely on initialisation.
	else if (falling[idx].baddy != BL)
	{
		*xb = 0.0f;
		*yb = 0.0f;
		*dir = 0.0f;
	}
	// Otherwise return the falling barrel details.
	else
	{
		*xb = falling[idx].x;
		*yb = falling[idx].y;
		*dir = falling[idx].dir;
	}
}

// Get Marios current position and the direction that he is facing to support animation.
// facing L for left, R for right, U for up/down (going up or down a ladder) and space means not moving, so no need to animate.
// jump true for jumping.
void getMario(float *xm, float *ym, char *face, bool *jump)
{
	*xm   = Mario.x;
	*ym   = Mario.y;
	*face = Mario.face;
	*jump = Mario.jump;
}

