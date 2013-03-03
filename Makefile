# Makefile to build monte_pi_sprng program
# --- macros
COMPILER=nasm
# CFLAGS=  -O3 -I /usr/local/lib/sprng/include -I /usr/local/lib/pgplot -g
# OBJECTS= monte_pi_sprng.o plot.o
# LIBS = -L/usr/local/lib/sprng/lib -llcg -L/usr/local/lib/pgplot -lcpgplot -lpgplot -lX11 -lftn -lm


# --- targets
all: carmichael
carmichael: carmichael.asm
	$(COMPILER) carmichael.asm -o carmichael.com

# --- remove binary and executable files
clean:
	rm -f carmichael.com