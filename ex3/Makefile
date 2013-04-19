EX=3
COMPILER=g++
CFLAGS=-Wall
SOURCES = trainer.cpp
TARGETS = trainer.exe

all: $(TARGETS)

%.com: %.asm
	$(COMPILER) $< -o $@

trainer.exe: trainer.cpp
	$(COMPILER) $(CFLAGS) $< -o $@

tar: $(SOURCES) README
	tar -cf ex$(EX).tar $^

clean:
	rm -f $(TARGETS)

messagebox: messagebox.exe
