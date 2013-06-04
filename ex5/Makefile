EX=5
COMPILER=
CFLAGS=
SOURCES = peek.py parseSID.py cyanide.py mitm.py
EXTRAFILES = protocol.txt
TARGETS =

all: $(TARGETS)

tar: $(SOURCES) $(EXTRAFILES) README
	tar -cvf ex$(EX).tar $^

clean:
	rm -f $(TARGETS)
