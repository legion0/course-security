#!/usr/bin/env python
import sys, os, struct

SCRIPT_DIR, SELF_NAME = os.path.split(sys.argv[0])

USAGE_MSG = "USAGE: %s <SID>" % SELF_NAME
TIMEOUT = 2
MAGIC_PREFIX = ''.join([chr(x) for x in (0xDE, 0xAF)])
FIND_SERVER_PREFIX = MAGIC_PREFIX + chr(0x01)
GET_FILE_PREFIX = MAGIC_PREFIX + chr(0x03)
CWD = os.getcwd()
RECV_BUFFER_SIZE = 1048576 # 1MB

def main(args):
	try:
		sid = long(args[0])
	except:
		_die(USAGE_MSG)
	mac_address_suffix = ((sid & 3) << 16) | ((sid >> 2) & 0xFFFF)
	print "last 18 bits of mac address: 0x%06x" % mac_address_suffix
	dwMajorVersion = (sid >> 18) & 7
	print "dwMajorVersion: 0x%08x" % dwMajorVersion
	dwMinorVersion = (sid >> 21) & 7
	print "dwMinorVersion: 0x%08x" % dwMinorVersion
	wServicePackMajor = (sid >> 24) & 7
	print "wServicePackMajor: 0x%08x" % wServicePackMajor
	wServicePackMinor = (sid >> 27) & 7
	print "wServicePackMinor: 0x%08x" % wServicePackMinor
	wProductType = (sid >> 30) & 7
	print "wProductType: 0x%08x" % wProductType

def _die(msg, exitcode = -1):
	print >> sys.stderr, "Error: %s" % msg
	exit(exitcode)

if __name__ == "__main__":
	main(sys.argv[1:])
