#!/usr/bin/env python
import sys, re
ss = (
	"Go away debugger!",
	"Evil!",
)
for s in ss:
	sys.stdout.write("char STR_%s[] = {" % re.sub("[^a-zA-Z0-9]+", "_", s).strip("_").upper())
	for c in s:
		print "0x%02x, " % (ord(c) ^ 0x11),
	print "0x00};"