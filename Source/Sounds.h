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
	LEVEL2    = 6,
	LEVEL4    = 7,
	OVER      = 8,
};

// Call this to set up sounds once, before attempting to call putsoundSel.
extern void setupSound();

// Call this with one of the above SOUNDSEL for that sound action.
extern void putsoundSel(soundsel_t sndSel);

// Call this once before exiting to close down sound.
extern void QuitSound();
