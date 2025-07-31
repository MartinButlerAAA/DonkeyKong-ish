// DonkeyKong-ish game. Main program which calls some functions from Game to cover background and sprite movement.

#include <coreinit/screen.h>	// for OSScreen.
#include <coreinit/thread.h>	// for Sleep.
#include <coreinit/time.h>		// To get time in usec.
#include <vpad/input.h>			// For the game pad inputs.
#include <whb/proc.h>			// For the loop and to do home button correctly.
#include <whb/log.h>			// ** Using the console logging features seems to help set up the screen output.
#include <whb/log_console.h>	// ** Found neeeded to keep these in the build for the program to display properly.

#include "Game.h"				// For Game accessors and processing.
#include "Draw.h"				// For drawing via OSScreen.
#include "Images/Images.h"		// For the images to be drawn using Draw.h.
#include "Sounds.h"				// For sounds and background music.

#define COLOUR 0xF0F0F000u		// Almost white (for lines).

static unsigned int level;		// Game level (1 to 4).
static unsigned int cnt;		// Count to animation.
static unsigned int gameState;	// Game state 0 initialisation, 1 waiting, 2 playing, 3 game over
static bool	dead = false;		// Mario dead.
bool enableBarrels = false;		// Flag to enable falling barrels once game completed first time.

void drawBorder()
{
	// Put a border round the screen to make a neat edge.
	drawLine(XOFFSET, YOFFSET, XOFFSET + XDISPMAX, YOFFSET, COLOUR);
	drawLine(XOFFSET, YOFFSET - 1, XOFFSET + XDISPMAX, YOFFSET - 1, COLOUR);

	drawLine(XOFFSET, YOFFSET + YDISPMAX, XOFFSET + XDISPMAX, YOFFSET + YDISPMAX, COLOUR);
	drawLine(XOFFSET, YOFFSET + YDISPMAX + 1, XOFFSET + XDISPMAX, YOFFSET + YDISPMAX + 1, COLOUR);

	drawLine(XOFFSET, YOFFSET, XOFFSET, YOFFSET + YDISPMAX, COLOUR);
	drawLine(XOFFSET - 1, YOFFSET, XOFFSET - 1, YOFFSET + YDISPMAX, COLOUR);

	drawLine(XOFFSET + XDISPMAX, YOFFSET, XOFFSET + XDISPMAX, YOFFSET + YDISPMAX, COLOUR);
	drawLine(XOFFSET + XDISPMAX + 1, YOFFSET, XOFFSET + XDISPMAX + 1, YOFFSET + YDISPMAX, COLOUR);

	// If in the waiting state add instruction to press buttons to start.
	if (gameState == 1)
	{
		drawTextCentred("Press ZL and ZR to play", 0xFEFEFE00u, 3, XDISPCTR, YDISPCTR, SCREEN_TV);
	}
	// If game complete show this and instruction to press button to play again.
	else if (gameState == 3)
	{
		drawTextCentred("Congratulations!", 0xFEFEFE00u, 6, XDISPCTR, YDISPCTR/3, SCREEN_TV);
		drawTextCentred("Press ZL and ZR to play again", 0xFEFEFE00u, 3, XDISPCTR, YDISPCTR, SCREEN_TV);
	}
}

// Display the game background (game table).
void drawGame()
{
	char blk;	// Returned blk type used to select inage for display.

	// Keep count cycling for animation (used in other functions as well).
	cnt++;
	if (cnt > 7) { cnt = 0; }

	// Use the getGame API to get the entire contents of the game store to display the game level on the screen.
	for (unsigned int y = 0; y < GAMEY; y++)
	{
		for (unsigned int x = 0; x < GAMEX; x++)
		{
			blk = getGame(x, y);
			if (blk == LD) { drawImage(BLKSIZE, BLKSIZE, LadderImage, x * BLKSIZE, y * BLKSIZE); }
			else if (blk == L2) { drawImage(BLKSIZE, BLKSIZE, GirdladImage, x * BLKSIZE, y * BLKSIZE); }
			else if (blk == GD) { drawImage(BLKSIZE, BLKSIZE, GirderImage, x * BLKSIZE, y * BLKSIZE); }
			else if (blk == PH) { drawImage(BLKSIZE, BLKSIZE, PeachImage, x * BLKSIZE, y * BLKSIZE); }
			else if (blk == BR) { drawImage(BLKSIZE, BLKSIZE, BarrelImage, x * BLKSIZE, y * BLKSIZE); }
			else if (blk == DK) { drawImage(BLKSIZE * 2, BLKSIZE * 2, DKImage, x * BLKSIZE, y * BLKSIZE); }	// DK is 4 blocks 64x64
			else if (blk == PI) { drawImage(BLKSIZE * 2, BLKSIZE, PieImage, x * BLKSIZE, y * BLKSIZE); }	// Pie is two blocks
			else if ((blk == FL) && (cnt >= 6)) { drawImage(BLKSIZE * 2, BLKSIZE * 2, Flame1Image, x * BLKSIZE, y * BLKSIZE); }	// Flame is 4 blocks
			else if ((blk == FL) && (cnt >= 4)) { drawImage(BLKSIZE * 2, BLKSIZE * 2, Flame2Image, x * BLKSIZE, y * BLKSIZE); }
			else if ((blk == FL) && (cnt >= 2)) { drawImage(BLKSIZE * 2, BLKSIZE * 2, Flame3Image, x * BLKSIZE, y * BLKSIZE); }
			else if ((blk == FL) && (cnt >= 0)) { drawImage(BLKSIZE * 2, BLKSIZE * 2, Flame2Image, x * BLKSIZE, y * BLKSIZE); }
			else if (blk == QM) { drawImage(BLKSIZE * 2, BLKSIZE * 2, QMarkImage, x * BLKSIZE, y * BLKSIZE); }	// QMark is 4 blocks
		}
	}
}

// Display the baddy sprites.
void drawBaddies()
{
	char blk;					// Returned blk type used to select image for display.
	float xb, yb, dir;			// Returned positions within the game table and direction.

	// Go through all of the baddies to display their current position. 
	// In the case of the barrels, use the direction to do the correct rolling animation.
	for (unsigned int a = 0; a < MAXBADDY; a++)
	{
		getBaddy(a, &xb, &yb, &blk, &dir);
		if (blk == GB)			// Display Goomba, use cnt to animate walking.
		{
			if (cnt >= 6)      { drawImage(BLKSIZE, BLKSIZE, Goomba1Image, (unsigned int)(xb * (float)BLKSIZE), (unsigned int)(yb * (float)BLKSIZE)); }
			else if (cnt >= 4) { drawImage(BLKSIZE, BLKSIZE, Goomba2Image, (unsigned int)(xb * (float)BLKSIZE), (unsigned int)(yb * (float)BLKSIZE)); }
			else if (cnt >= 2) { drawImage(BLKSIZE, BLKSIZE, Goomba1Image, (unsigned int)(xb * (float)BLKSIZE), (unsigned int)(yb * (float)BLKSIZE)); }
			else               { drawImage(BLKSIZE, BLKSIZE, Goomba3Image, (unsigned int)(xb * (float)BLKSIZE), (unsigned int)(yb * (float)BLKSIZE)); }
		}
		else if (blk == BL)		// Display Barrel, use cnt to sequence rolling animation.
		{
			if (dir > 0.0f)	// Rotate for rolling right.
			{
				if (cnt >= 6)      { drawImage(BLKSIZE, BLKSIZE, Barrel4Image, (unsigned int)(xb * (float)BLKSIZE), (unsigned int)(yb * (float)BLKSIZE)); }
				else if (cnt >= 4) { drawImage(BLKSIZE, BLKSIZE, Barrel3Image, (unsigned int)(xb * (float)BLKSIZE), (unsigned int)(yb * (float)BLKSIZE)); }
				else if (cnt >= 2) { drawImage(BLKSIZE, BLKSIZE, Barrel2Image, (unsigned int)(xb * (float)BLKSIZE), (unsigned int)(yb * (float)BLKSIZE)); }
				else               { drawImage(BLKSIZE, BLKSIZE, Barrel1Image, (unsigned int)(xb * (float)BLKSIZE), (unsigned int)(yb * (float)BLKSIZE)); }
			}
			else // Rotate for rolling left.
			{
				if (cnt >= 6)      { drawImage(BLKSIZE, BLKSIZE, Barrel1Image, (unsigned int)(xb * (float)BLKSIZE), (unsigned int)(yb * (float)BLKSIZE)); }
				else if (cnt >= 4) { drawImage(BLKSIZE, BLKSIZE, Barrel2Image, (unsigned int)(xb * (float)BLKSIZE), (unsigned int)(yb * (float)BLKSIZE)); }
				else if (cnt >= 2) { drawImage(BLKSIZE, BLKSIZE, Barrel3Image, (unsigned int)(xb * (float)BLKSIZE), (unsigned int)(yb * (float)BLKSIZE)); }
				else               { drawImage(BLKSIZE, BLKSIZE, Barrel4Image, (unsigned int)(xb * (float)BLKSIZE), (unsigned int)(yb * (float)BLKSIZE)); }
			}
		}
	}
}

// Display the falling barrel sprites.
void drawFallingBarrels()
{
	float xb, yb, dir;			// Returned positions within the game table and direction.

	// Go through all of the falling barrels to display their current position. 
	// In the case of the barrels, use the direction to do the correct rolling animation.
	for (unsigned int a = 0; a < MAXBADDY; a++)
	{
		// Get the position and direction for a falling barrel.
		getFallingBarrel(a, &xb, &yb, &dir);

		// If the barrel is initialised (i.e. is an actual barrel).
		if ((xb > 0.0f) && (yb > 0.0f))
		{
			if (dir > 0.0f)	// Rotate for rolling right.
			{
				if (cnt >= 6) { drawImage(BLKSIZE, BLKSIZE, Barrel4Image, (unsigned int)(xb * (float)BLKSIZE), (unsigned int)(yb * (float)BLKSIZE)); }
				else if (cnt >= 4) { drawImage(BLKSIZE, BLKSIZE, Barrel3Image, (unsigned int)(xb * (float)BLKSIZE), (unsigned int)(yb * (float)BLKSIZE)); }
				else if (cnt >= 2) { drawImage(BLKSIZE, BLKSIZE, Barrel2Image, (unsigned int)(xb * (float)BLKSIZE), (unsigned int)(yb * (float)BLKSIZE)); }
				else { drawImage(BLKSIZE, BLKSIZE, Barrel1Image, (unsigned int)(xb * (float)BLKSIZE), (unsigned int)(yb * (float)BLKSIZE)); }
			}
			else // Rotate for rolling left.
			{
				if (cnt >= 6) { drawImage(BLKSIZE, BLKSIZE, Barrel1Image, (unsigned int)(xb * (float)BLKSIZE), (unsigned int)(yb * (float)BLKSIZE)); }
				else if (cnt >= 4) { drawImage(BLKSIZE, BLKSIZE, Barrel2Image, (unsigned int)(xb * (float)BLKSIZE), (unsigned int)(yb * (float)BLKSIZE)); }
				else if (cnt >= 2) { drawImage(BLKSIZE, BLKSIZE, Barrel3Image, (unsigned int)(xb * (float)BLKSIZE), (unsigned int)(yb * (float)BLKSIZE)); }
				else { drawImage(BLKSIZE, BLKSIZE, Barrel4Image, (unsigned int)(xb * (float)BLKSIZE), (unsigned int)(yb * (float)BLKSIZE)); }
			}
		}
	}
}

// Display Mario including animation.
void drawMario()
{ 
	float xm, ym;				// Returned position of Mario.
	char face;					// Returned direction Mario is facing.
	bool jump;					// Returned Mario jumping.

	// Get Mario position then display.
	getMario(&xm, &ym, &face, &jump);
	if (face == ' ')		// Standing still.
	{
		drawImage(BLKSIZE, BLKSIZE, Mario1Image, (unsigned int)(xm * (float)BLKSIZE), (unsigned int)(ym * (float)BLKSIZE));
	}
	else if (face == 'R')	// Moving right
	{
		if (cnt >= 6)      { drawImage(BLKSIZE, BLKSIZE, Mario1Image, (unsigned int)(xm * (float)BLKSIZE), (unsigned int)(ym * (float)BLKSIZE)); }
		else if (cnt >= 4) { drawImage(BLKSIZE, BLKSIZE, Mario2Image, (unsigned int)(xm * (float)BLKSIZE), (unsigned int)(ym * (float)BLKSIZE)); }
		else if (cnt >= 2) { drawImage(BLKSIZE, BLKSIZE, Mario1Image, (unsigned int)(xm * (float)BLKSIZE), (unsigned int)(ym * (float)BLKSIZE)); }
		else               { drawImage(BLKSIZE, BLKSIZE, Mario3Image, (unsigned int)(xm * (float)BLKSIZE), (unsigned int)(ym * (float)BLKSIZE)); }
	}
	else if (face == 'L')	// Moving left
	{
		if (cnt >= 6)      { drawImage(BLKSIZE, BLKSIZE, Mario4Image, (unsigned int)(xm * (float)BLKSIZE), (unsigned int)(ym * (float)BLKSIZE)); }
		else if (cnt >= 4) { drawImage(BLKSIZE, BLKSIZE, Mario5Image, (unsigned int)(xm * (float)BLKSIZE), (unsigned int)(ym * (float)BLKSIZE)); }
		else if (cnt >= 2) { drawImage(BLKSIZE, BLKSIZE, Mario4Image, (unsigned int)(xm * (float)BLKSIZE), (unsigned int)(ym * (float)BLKSIZE)); }
		else               { drawImage(BLKSIZE, BLKSIZE, Mario6Image, (unsigned int)(xm * (float)BLKSIZE), (unsigned int)(ym * (float)BLKSIZE)); }
	}
	else if (face == 'U')	// Moving up or down a ladder (looks the same).
	{
		if (cnt >= 6)      { drawImage(BLKSIZE, BLKSIZE, Mario7Image, (unsigned int)(xm * (float)BLKSIZE), (unsigned int)(ym * (float)BLKSIZE)); }
		else if (cnt >= 4) { drawImage(BLKSIZE, BLKSIZE, Mario8Image, (unsigned int)(xm * (float)BLKSIZE), (unsigned int)(ym * (float)BLKSIZE)); }
		else if (cnt >= 2) { drawImage(BLKSIZE, BLKSIZE, Mario7Image, (unsigned int)(xm * (float)BLKSIZE), (unsigned int)(ym * (float)BLKSIZE)); }
		else               { drawImage(BLKSIZE, BLKSIZE, Mario9Image, (unsigned int)(xm * (float)BLKSIZE), (unsigned int)(ym * (float)BLKSIZE)); }
	}
}

// Display all elements from the game on the TV screen.
void displayTV()
{
	// Clear the TV to have a fixed background.
	OSScreenClearBufferEx(SCREEN_TV, BKGNDCLR);

	// Only display the actual game screens while the game is playing.
	if (gameState == 2)
	{
		drawGame();				// Display the game array to put the game level background on the screen.
		drawBaddies();			// Add moving baddies.
		drawFallingBarrels();	// Add falling barrels to the screen.
		drawMario();			// Add Mario.
	}
	drawBorder();	// Put a border round the play area of the screen.

	// Show if Mario has died.
	if (dead == true)
	{
		drawTextCentred("Game Over!", 0xFE000000u, 5, XDISPCTR, YDISPCTR, SCREEN_TV);
	}

	// Flip the screen buffer to show the new display.
	OSScreenFlipBuffersEx(SCREEN_TV);
	return;
}

// Display information on the Gamepad screen.
void displayGPad()
{
	// Clear the Gamepad screen.
	OSScreenClearBufferEx(SCREEN_DRC, BKGNDCLR);

	drawText("Donkey Kong-ish", 0xFEFEFE00u, 5, 10, 10, SCREEN_DRC);
	drawText("There's no score, just get to the end.", 0xFEFEFE00u, 2, 10, 100, SCREEN_DRC);
	drawText("Jump for the question mark for the next level.", 0xFEFEFE00u, 2, 10, 130, SCREEN_DRC);
	drawText("Left joycon or direction buttons to move.", 0xFEFEFE00u, 2, 10, 200, SCREEN_DRC);
	drawText("X to jump.", 0xFEFEFE00u, 2, 10, 230, SCREEN_DRC);

	// Flip the screen buffer to show the new display.
	OSScreenFlipBuffersEx(SCREEN_DRC);
	return;
}

// Allows for pass parameters as per standard C, but these are never used.
int main(int argc, char **argv) 
{
	VPADStatus status;		// Status returned for the gamepad button.
	VPADReadError error;	// Error from gamepad.

	char move = ' ';		// Direction of Mario movement from gamepad.
	bool jump = false;		// If Mario commanded to jump.
	char ret = ' ';			// Reurn value from Mario move.

	int tim;				// Variable for processing time of a game cycle.
	OSTime tm1, tm2;		// Times in usec used to time a game cycle.
	int del = 5;			// Delay used to sequence the game.

    WHBProcInit();			// This is the main process and must be in the program at the start for the home button to operate correctly.
    WHBLogConsoleInit();	// ConsoleInit seems to get the display to operate correctly so keep in the build.

	setupSound();			// Call separate function from sound.h to do all of the sound setup.

	gameState = 0;			// Initialisation state.

	// There must be a main loop on WHBProc running, for the program to correctly operate with the home button.
	// Home pauses this loop and continues it if resume is selected. There must therefore be one main loop of processing in the main program.
	while (WHBProcIsRunning()) 
	{
		tm1 = OSTicksToMicroseconds(OSGetTick());	// Time before a game cycle processing.

		if (gameState == 0)			// Initialise
		{
			putsoundSel(STOPBKGND);	// Ensure the background game music is stopped.
			putsoundSel(START);		// Get the music for the start screen going.
			gameState = 1;			// Move from initialisation to waiting to start.
		}
		else if (gameState == 1)	// Wait to play.
		{
			dead = false;
			VPADRead(VPAD_CHAN_0, &status, 1, &error);	// Get the VPAD button last pressed.
			if (error == VPAD_READ_SUCCESS)				// Only process buttons if no errors (e.g. gamepad lost power).
			{
				// Capture the controls to see if game to start.
				if ((status.hold & VPAD_BUTTON_ZL) && (status.hold & VPAD_BUTTON_ZR))
				{
					gameState = 2;			// Move from waiting to playing.
					putsoundSel(INTRO);		// Play the introduction.
					OSSleepTicks(OSMillisecondsToTicks(5000));	// Wait for intro music to play.
					putsoundSel(STRTBKGND);	// Play background music while playing.

					level = 1;				// Initialise the game to the first level.
					initGame(level);
				}
			}
		}
		else if (gameState == 2)	// Play
		{
			move = ' ';		// Clear so only updated for button presses.
			jump = false;

			VPADRead(VPAD_CHAN_0, &status, 1, &error);	// Get the VPAD button last pressed.
			if (error == VPAD_READ_SUCCESS)				// Only process buttons if no errors (e.g. gamepad lost power).
			{
				// Capture the direction and jump controls to pass into the Game.
				if ((status.hold & VPAD_BUTTON_UP) || (status.hold & VPAD_STICK_L_EMULATION_UP)) { move = 'U'; }
				if ((status.hold & VPAD_BUTTON_DOWN) || (status.hold & VPAD_STICK_L_EMULATION_DOWN)) { move = 'D'; }
				if ((status.hold & VPAD_BUTTON_LEFT) || (status.hold & VPAD_STICK_L_EMULATION_LEFT)) { move = 'L'; }
				if ((status.hold & VPAD_BUTTON_RIGHT) || (status.hold & VPAD_STICK_L_EMULATION_RIGHT)) { move = 'R'; }
				if (status.trigger & VPAD_BUTTON_X) { jump = true; }
			}

			moveBaddies();	// Move the baddies within the Game.
			ret = moveMario(move, jump);	// Move Mario including taking play commands.

			// Move on a level if got to the end.
			if (ret == 'E')
			{
				putsoundSel(STOPBKGND);							// Stop background music.
				OSSleepTicks(OSMillisecondsToTicks(20));		// Wait for background to stop.
				if (level == 1)
				{
					putsoundSel(LEVEL1);						// Play level complete.
					OSSleepTicks(OSMillisecondsToTicks(3800));	// Wait for music to play.
					putsoundSel(STRTBKGND);						// Start background music for next level.

					level = 2;									// Initialise next level.
					initGame(level);
				}
				else if (level == 2)
				{
					putsoundSel(LEVEL2);						// Play level complete.
					OSSleepTicks(OSMillisecondsToTicks(2900));	// Wait for music to play.
					putsoundSel(STRTBKGND);						// Start background music for next level.

					level = 3;									// Initialise next level.
					initGame(level);
				}
				else if (level == 3)
				{
					putsoundSel(LEVEL3);						// Play level complete.
					OSSleepTicks(OSMillisecondsToTicks(7030));	// Wait for music to play.
					putsoundSel(STRTBKGND);						// Start background music for next level.

					level = 4;									// Initialise the next level.
					initGame(level);
				}
				else if (level == 4)
				{
					putsoundSel(LEVEL4);						// Play level complete.
					OSSleepTicks(OSMillisecondsToTicks(7080));	// Wait for music to play.
					putsoundSel(START);							// Play waiting music.

					gameState = 3;								// Go to end of game state.
				}
			}

			// This is done before checking the return value to allow one extra iteration to update the screen once Mario is dead.
			if (dead == true)
			{
				putsoundSel(STOPBKGND);						// Stop background music
				OSSleepTicks(OSMillisecondsToTicks(20));	// Wait for background to stop.
				putsoundSel(OVER);							// Play game over.
				OSSleepTicks(OSMillisecondsToTicks(3400));	// Wait for music to play.

				gameState = 0;								// Go back to re-initialise.			
			}
			// Check if Mario has died.
			if (ret == 'D')
			{
				dead = true;
			}
		}
		else if (gameState == 3)	// Game complete state.
		{
			enableBarrels = true; // Enable barrels for the next time through.

			VPADRead(VPAD_CHAN_0, &status, 1, &error);	// Get the VPAD button last pressed.
			if (error == VPAD_READ_SUCCESS)				// Only process buttons if no errors (e.g. gamepad lost power).
			{
				// Capture the direction and jump controls to pass into the Game.
				if ((status.hold & VPAD_BUTTON_ZL) && (status.hold & VPAD_BUTTON_ZR))
				{
					gameState = 2;			// Move to playing.
					putsoundSel(INTRO);		// Play the introduction.
					OSSleepTicks(OSMillisecondsToTicks(5000));	// Wait for intro music to play.
					putsoundSel(STRTBKGND);						// Start background music

					level = 1;				// Initialise the game to the first level.
					initGame(level);
				}
			}
		}

		displayTV();	// Update the TV display.
		displayGPad();	// Update the Gamepad display.

		tm2 = OSTicksToMicroseconds(OSGetTick());	// Time after game cycle processing.

		// The screen update rate is 60Hz, 50ms is 3 screen updates, so the game is running at an update rate of 20Hz.
		tim = ((tm2 - tm1) / 1000);					// Calulate the processing time in msec.
		del = 50 - tim;								// Adjust the game delay for the amount of time used in processing.
		if (del <= 0) { del = 3; }					// Limit delay to sensible values to avoid program getting stuck.
		if (del > 50) { del = 50; }
		OSSleepTicks(OSMillisecondsToTicks(del));	// Delay to keep game operating at the same screen update.
	}

	QuitSound();

	// If we get out of the program clean up and exit.
    WHBLogConsoleFree();
    WHBProcShutdown();
    return 0;
}