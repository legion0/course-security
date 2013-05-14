EX=4
COMPILER=
CFLAGS=
SOURCES = cptnhook.c cptnhook.h cptnhook.def peterpan.c peterpan.h
EXTRAFILES =
TARGETS =

all: $(TARGETS)

tar: $(SOURCES) $(EXTRAFILES) README
	tar -cf ex$(EX).tar $^

clean:
	rm -f $(TARGETS)
