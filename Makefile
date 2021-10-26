build:
	gcc -O2 -std=c11 term.c -o term $$(pkg-config --cflags vte-2.91) $$(pkg-config --libs vte-2.91)

install:
	cp -f ./term /home/matteo/.local/bin/vterm

clean:
	rm ./term
