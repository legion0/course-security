#!/usr/bin/env python
import sys, os
def main(argv):
#	unmask(); exit(0)
	mask(); exit(0)

def mask():
	content = [x for x in readBytes("q3_clean.bin")]

	decrypter = content[0:0x22]
	exploit = content[0x22:0xC0]
	data = content[0xC0:0x10C]
	jumper = content[0x10C:0x10F]

	xorChar = decrypter[0x1B]

	exploit = [x^xorChar for x in exploit]
	data = [x^xorChar for x in data]

	content = decrypter + exploit + data + jumper
	writeBytes("q3.bin", content)

def unmask():
	content = [x for x in readBytes("q3.bin")]

	decrypter = content[0:0x22]
	exploit = content[0x22:0xC0]
	data = content[0xC0:0x104]
	jumper = content[0x104:0x10F]

	xorChar = decrypter[0x1B]

	exploit = [x^xorChar for x in exploit]
	data = [x^xorChar for x in data]

	content = decrypter + exploit + data + jumper
	writeBytes("q3_clean.bin", content)

def readBytes(filename, chunksize=8192):
    with open(filename, "rb") as f:
        while True:
            chunk = f.read(chunksize)
            if chunk:
                for b in chunk:
                    yield ord(b)
            else:
                break
def writeBytes(filename, content):
    with open(filename, "wb") as f:
        for b in content:
			f.write(chr(b))

if __name__ == "__main__":
	main(sys.argv[1:])
