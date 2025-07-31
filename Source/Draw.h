// Header for drawing functions.
#pragma once

#include <stdbool.h>	// To use bools

#include "Game.h"		// For the GAMEX and GAMEY constants for the size of the game screens.

// The background colour is defined. Pixels in images that match the background colour are not processed, in effect giving sprites a transparent background.
#define BKGNDCLR 0x00000000l

#define BLKSIZE 32						// Size of sprites and main building blocks of game.

#define XDISPMAX (GAMEX * BLKSIZE)		// Limits to game screen size.
#define YDISPMAX (GAMEY * BLKSIZE)

#define XOFFSET  (1240 - XDISPMAX) / 2	// Offset to position game screen centrally on TV.
#define YOFFSET  ( 720 - YDISPMAX) / 2

#define XDISPCTR XDISPMAX/2				// Centre of the game screen.
#define YDISPCTR YDISPMAX/2

// Draw a line of the colour specified.
bool drawLine(float x1, float y1, float x2, float y2, unsigned int colour);

// Draw an image on the screen at xpos and ypos.
bool drawImage(unsigned int xmax, unsigned int ymax, unsigned int ImageP[ymax][xmax], unsigned int xpos, unsigned int ypos);

// Draw text at the position specified.
bool drawText(char* dispString, unsigned int colour, unsigned int scale, unsigned int xpos, unsigned int ypos, unsigned int display);

// Draw text centred on the position specified.
bool drawTextCentred(char* dispString, unsigned int colour, unsigned int scale, unsigned int xpos, unsigned int ypos, unsigned int display);

//
// THE FOLLOWING ARE IN DRAW FOR GENERAL USE, BUT ARE NOT NEEDED FOR DONKEYKONG-ISH
//

#define ZMAX 36		// Number of images for rotating display (in this case an image per 10 degrees).

// Initialise 3D array for rotating image.
bool initialiseRotatingImage(unsigned int xmax, unsigned int ymax, unsigned int ImageP[ymax][xmax], unsigned int ImagesP[ZMAX][ymax][xmax]);

// Draw a rotated image.
bool rotateImage(unsigned int xmax, unsigned int ymax, unsigned int ImageP[ZMAX][ymax][xmax], unsigned int xpos, unsigned int ypos, unsigned int angle);

// Draw the visible part of the background based on xpos and ypos.
bool drawBackground(unsigned int xmax, unsigned int ymax, unsigned int ImageP[ymax][xmax], unsigned int xpos, unsigned int ypos);

// Draw a scaled image on the screen at xpos and ypos. Scaled between 0% and 100%.
bool scaleImage(unsigned int xmax, unsigned int ymax, unsigned int ImageP[ymax][xmax], unsigned int xpos, unsigned int ypos, float pct);
