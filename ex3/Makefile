EX=3
COMPILER=g++
CFLAGS=-Wall
SOURCES = trainer.c trainer.h
EXTRAFILES = winmine_freezed.exe winmine_extreme.exe winmine_countdown.exe wackymine.exe godlymine.exe winmine.idb
TARGETS = trainer.exe

all: $(TARGETS)

trainer.exe: trainer.c trainer.h
	$(COMPILER) $(CFLAGS) $< -o $@

tar: $(SOURCES) $(EXTRAFILES) README
	tar -cf ex$(EX).tar $^

clean:
	rm -f $(TARGETS)
