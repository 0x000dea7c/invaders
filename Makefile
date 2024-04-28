#
# if you were on windows or any other platform the compilation cmd will change to something like:
#
# [g++ -ggdb -Wall -Wextra -Iinc] src/windows_invaders.cpp [-lX11 -lGL -o invaders]
# [g++ -ggdb -Wall -Wextra -Iinc] src/macos_invaders.cpp [-lX11 -lGL -o invaders]
#
# etc; you'd need to implement these files, though; that's intended, bc each platform handles
# things differently
#
all:
	find . -name '*.c' -o -name '*.cpp' -o -name '*.h' -o -name '*.hpp' -type f | xargs etags && g++ -ggdb -Wall -Wextra -Iinc -I/usr/include/freetype2 src/gnulinux_invaders.cpp -lX11 -lGL -lfreetype -o invaders

clean:
	rm invaders

run:
	./invaders
