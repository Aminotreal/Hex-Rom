HexRom: main.c
	gcc -O3 -march='native' -mtune='native' -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -o HexRom main.c

debug: main.c
	gcc -O3 -march='native' -mtune='native' -g3 -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -o HexRom.debug main.c
