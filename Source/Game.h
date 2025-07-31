// Header for DonkeyKong-ish accessors.
#pragma once

#include <stdbool.h>	// To use bools

// These are the game array sizes used to describe the game screen.
#define GAMEX 26	// Game array x sixe (columns)
#define GAMEY 20	// Game array y size (rows)

// The constants for the different components of the gamescreen, used for display and to control basic game play.
#define SP  0		// Blank space
#define LD 10		// Ladder
#define L2 11		// Ladder Top (to join with girder)
#define GD 20		// Girder
#define PH 30		// Peach
#define BR 40		// Barrel
#define DK 50		// Donkey Kong
#define DI 51		// Die blocks, donkey kong, pie, flames or fall will kill Mario
#define PI 60		// Pie
#define FL 70		// Flame
#define QM 80		// Question Mark
#define Q2 81		// Hidden parts of question mark
#define EG 90		// Edge Marker (blank)

// Baddies types
#define GB 100		// Goomba
#define BL 110		// Moving barrel

#define MAXBADDY 7	// Maximum number of baddies (goombers or barrels) in baddies array at one time.

void initGame(unsigned int level);				// Initialise the game level.

void moveBaddies(void);							// Move the goombas and/or barrels on the game board.

char moveMario(char move, bool jump);			// Move Mario, taking into account the current movement controls.
												// move 'U' up, 'D' down, 'L' left, 'R' right. Jump true for jump button pressed.
												// Returns 'D' for dead, 'E' for end of level or ' '.

char getGame(unsigned int x, unsigned int y);	// Get the block at a position in the game table (for display).

// Pass back the current x and y positions along with the baddy type for the baddy selected and direction.
// If the baddy is not used in this level baddy, xb and yb will all be set to 0.
// Likewise if the baddy selected is outside the range MAXBADDY the values returned will be 0.
void getBaddy(unsigned int idx, float *xb, float *yb, char *baddy, float *dir);

// Pass back the current x and y positions and direction for falling barrels.
// If the falling barrel is not used in this level xb and yb will all be set to 0.
// Likewise if the barrel selected is outside the range MAXBADDY the values returned will be 0.
void getFallingBarrel(unsigned int idx, float* xb, float* yb, float* dir);


// Get Marios current position and the direction that he is facing to support animation.
// facing L for left, R for right, U for up/down (going up or down a ladder) and space means not moving, so no need to animate.
// jump true for jumping.
void getMario(float *xm, float *ym, char *face, bool *jump);
