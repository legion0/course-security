COMPILER=nasm
LINKER=gcc
# CFLAGS=  -O3 -I /usr/local/lib/sprng/include -I /usr/local/lib/pgplot -g
# OBJECTS= monte_pi_sprng.o plot.o
# LIBS = -L/usr/local/lib/sprng/lib -llcg -L/usr/local/lib/pgplot -lcpgplot -lpgplot -lX11 -lftn -lm

TARGETS = carmichael.com game.com login.exe messagebox.exe

all: $(TARGETS)

%.com: %.asm
	$(COMPILER) $< -o $@

%.exe: %.obj
	$(LINKER) $< -o $@
%.obj : %.asm
	$(COMPILER) -f win32 $<

clean:
	rm -f $(TARGETS)
