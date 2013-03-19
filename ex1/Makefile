COMPILER=nasm
CFLAGS=-f win32
LINKER=gcc
SOURCES = carmichael.asm game.asm login.asm messagebox.asm
TARGETS = carmichael.com game.com login.exe messagebox.exe ex1.tar

all: $(TARGETS)

%.com: %.asm
	$(COMPILER) $< -o $@

%.exe: %.obj
	$(LINKER) $< -o $@
%.obj : %.asm
	$(COMPILER) $(CFLAGS) $<

ex1.tar: $(SOURCES) README
	tar -cf $@ $^

clean:
	rm -f $(TARGETS)
