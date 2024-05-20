# invaders

Second game in C++ and OpenGL with not too many dependencies

WIP - WORK IN PROGRESS

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

tbd

# Play

## GNU/Linux

Run: `make run`

## Windows

tbd

# Remove executable

Run: `make clean`

## Windows

tbd

# External dependencies

## GNU/LINUX

The idea was to not use any, but don't have much time left (life stuff) for audio, text and image processing.

- X11 included in most gnulinux distros
- SDL_mixer & SDL: need to install this separately
- Freetype (freetype2): probably need to install this separately
- libvorbis: probably need to install this separately

## WINDOWS

tbd

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
