rem This batch file runs the .bmp files through a tool that creates a .h header file for the .bmp file passed in.
rem The .h file produced is 2 dimensional array of 32-bit integers for 24 bit colour, in the correct format for
rem the WiiU OSScreen pixel function. 
rem
rem see https://github.com/MartinButlerAAA/ProcessBmp for the ProcessBmp tool.
rem
rem This tool can only handle .bmp images. Any other images need to be converted to 24-bit colour .bmp files, which can
rem be done using Microsoft Paint.
rem 
cd .
processBmp Mario1.bmp
processBmp Mario2.bmp
processBmp Mario3.bmp
processBmp Mario4.bmp
processBmp Mario5.bmp
processBmp Mario6.bmp
processBmp Mario7.bmp
processBmp Mario8.bmp
processBmp Mario9.bmp

processBmp Barrel.bmp
processBmp Barrel1.bmp
processBmp Barrel2.bmp
processBmp Barrel3.bmp
processBmp Barrel4.bmp

processBmp Goomba1.bmp
processBmp Goomba2.bmp
processBmp Goomba3.bmp

processBmp Flame1.bmp
processBmp Flame2.bmp
processBmp Flame3.bmp

processBmp Girder.bmp
processBmp Girdlad.bmp
processBmp Ladder.bmp

processBmp DK.bmp
processBmp Peach.bmp
processBmp Pie.bmp
processBmp QMark.bmp

pause

