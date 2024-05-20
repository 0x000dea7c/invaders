# invaders

Second game in C++ and OpenGL with not too many dependencies

Wanted to create a game that:

- compiled very easily (no need for convoluted build systems)
- compiled fast
- did batch rendering draws
- used int unique identifiers for game assets/resources
- used events to notify certain actions and thus reduce coupling between game files
- implemented a menu w/ sound regulation (actually did that in previous game too)
- implemented a scoring system
- implemented better interesection tests than previous game
- implemented a way to reduce number of intersection tests done between entities (using a spatial hash grid)
- portability between platforms wasn't hard (only add files like win32_invaders.cpp, macos_invaders.cpp and the rest shouldn't change)
- audio device selection in game
- ...

Note: apparently too dumb to implement decent intersection tests. This version is better than previous' game, but it still experiences tunneling. Wonder if the spatial hash grid implementation is decent, too. Anyway, let's keep improving.

Need to improve on themes like:

- intersection tests, now it sorta works, but theres' tunneling
- world representation in some ds
- adapting game's world to screen size dynamically
- code organisation
- in-game editor for manipulating objects
- overall optimisation
- animations
- inter text
- entities attrs defined in text files so you don't need to recompile the game (although it takes 3s to compile 10k lines atm, could be reduced)

Note 2: of course this is bloated because few months ago was writing hello world programs in C++

# Compilation

## GNU/Linux

Run: `make`

## Windows

You need Visual Studio Code and "run" (or whatever is called) the solution win32_invaders.sln under win32_invaders.

Process is a bit complex because you need to setup Visual Studio to grab SDL2, SDL2_mixer, Freetype and OpenGL deps (linker, include, sources), but if you're using Visual Studio, you probably know how to do it.

Also, you need to add as include directories the dir ./invaders/inc and as source directories ./invaders/src.

# Play

## GNU/Linux

Run: `make run`

## Windows

Open Visual Studio and play the run button, I guess.

# Remove executable

Run: `make clean`

## Windows

Remove the directory win32_invaders/x64 that Visual Studio creates when the game is compiled.

# External dependencies

## GNU/LINUX

The idea was to not use any, but don't have much time left (life stuff) for audio, text and image processing.

- X11 included in most gnulinux distros
- SDL2_mixer & SDL2: need to install this separately
- Freetype (freetype2): probably need to install this separately
- libvorbis: probably need to install this separately

## WINDOWS

You need:

- Visual Studio.
- SDL2 & SDL2_mixer: if you're on Windows you probably know how to integrate them with Visual Studio.
- Freetype: same thing as previous point.
- glad: it's already included in the win32_invaders directory.

NOTE: won't be uploading SDL2, SDL2_mixer libs and source files in the repo because then size will grow considerably. You're on your own here.

Couldn't be bothered with win32 API for creating the Window at the end because had a hard time finding a good tutorial and my time is running out.

Same thing with DSound.

# Art

## Textures/images

Most of them come from: https://kenney.nl/ and https://opengameart.org/.

Modified some of them to fit better in-game, but my art sucks.

Background: https://opengameart.org/content/space-background-1

Thank you for your generosity.

## Music and effects

- https://opengameart.org/content/space-background
- https://opengameart.org/content/big-explosion
- https://mixkit.co/free-sound-effects/game/
- https://kronbits.itch.io/freesfx
- https://www.zapsplat.com/sound-effect-category/game-music-and-loops/
- etc

Thank you for your generosity.

# Demo

## Compiling and running natively on GNU/Linux



## Compiling and running natively on Windows


https://www.youtube.com/watch?v=uxmYomtnjzk
