#pragma once
// The function interface to play sounds.

//! A value from enum SOUNDSEL.
typedef unsigned int soundsel_t;

enum SOUNDSEL
{
	NOCHANGE  = 0,
	STOPBKGND = 1,
	STRTBKGND = 2,
	START     = 3,		// Note that this is set up to loop, but will be stopped when intro music is selected.
	INTRO     = 4,
	JUMP      = 5,
	WALK      = 6,
	LEVEL1    = 7,
	LEVEL2    = 8,
	LEVEL3    = 9,
	LEVEL4    = 10,
	OVER      = 11,
};

// Call this to set up sounds once, before attempting to call putsoundSel.
extern void setupSound();

// Call this with one of the above SOUNDSEL for that sound action.
extern void putsoundSel(soundsel_t sndSel);

// Call this once before exiting to close down sound.
extern void QuitSound();
