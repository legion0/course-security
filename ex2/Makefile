COMPILER=nasm
CFLAGS=-f win32
LINKER=gcc
SOURCES = q2.txt q2.bin q3.txt q3.bin
TARGETS = ex2.tar

all: $(TARGETS)

%.com: %.asm
	$(COMPILER) $< -o $@

%.exe: %.obj
	$(LINKER) $< -o $@
%.obj : %.asm
	$(COMPILER) $(CFLAGS) $<

ex2.tar: $(SOURCES) README
	tar -cf $@ $^

clean:
	rm -f $(TARGETS)
