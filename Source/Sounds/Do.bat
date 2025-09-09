rem Martin Butler 09/09/2025
rem This batch file calls the ProcessWav tool that takes an 8-bit 48000 samples per second .wav files and creates a header 
rem file of the data. WavePad from NCH software can be used to save .mp3 files as .wav in the correct format.
rem see https://github.com/MartinButlerAAA/ProcessWav for the ProcessWav tool.
rem
rem The tool converts from 8-bit .wav format (which is 0 to 255 with centre at 128) to the Wii U format which is -128 to 127.
rem If the raw data is used without conversion, it gives a horribly distorted sound.

cd .
ProcessWav Start.wav
ProcessWav Intro.wav
ProcessWav Background.wav
ProcessWav Jump.wav
ProcessWav Level2.wav
ProcessWav Level4.wav
ProcessWav GameOver.wav

Pause