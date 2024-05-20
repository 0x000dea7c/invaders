#
# if you were on windows or any other platform the compilation cmd will change to something like:
#
# [g++ -Wall -Wextra -Iinc] src/windows_invaders.cpp [-lX11 -lGL -o invaders]
# [g++ -Wall -Wextra -Iinc] src/macos_invaders.cpp [-lX11 -lGL -o invaders]
#
# etc; you'd need to implement these files, though; that's intended, bc each platform handles
# things differently
#
all:
	g++ -fno-builtin -O2 -fno-exceptions -fno-rtti -Wall -Wextra -Iinc -I/usr/include/freetype2 src/gnulinux_invaders.cpp -lX11 -lGL -lfreetype -lSDL2 -lSDL2_mixer -o invaders

clean:
	rm invaders

run:
	./invaders
