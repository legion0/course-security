CC=gcc
CFLAGS=
SOURCES = getProcAddress.c
TARGETS = getProcAddress.exe

all: $(TARGETS)

%.exe: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGETS)

