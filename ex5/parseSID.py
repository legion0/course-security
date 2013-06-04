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

PRODUCT_TYPE_MAP = (
	"VER_NT_WORKSTATION",
	"VER_NT_DOMAIN_CONTROLLER",
	"VER_NT_SERVER",
)
OS_TABLE = (
	("Windows 8", 							6, 2, lambda wProductType: wProductType == 1),
	("Windows Server 2012", 				6, 2, lambda wProductType: wProductType != 1),
	("Windows 7", 							6, 1, lambda wProductType: wProductType == 1),
	("Windows Server 2008 R2", 				6, 1, lambda wProductType: wProductType != 1),
	("Windows Server 2008", 				6, 0, lambda wProductType: wProductType != 1),
	("Windows Vista", 						6, 0, lambda wProductType: wProductType == 1),
	("Windows Server 2003 R2", 				5, 2, lambda _: True),
	("Windows Home Server", 				5, 2, lambda _: True),
	("Windows Server 2003", 				5, 2, lambda _: True),
	("Windows XP Professional x64 Edition", 5, 2, lambda wProductType: wProductType == 1),
	("Windows XP", 							5, 1, lambda _: True),
	("Windows 2000", 						5, 0, lambda _: True),
)

def main(args):
	try:
		sid = long(args[0])
	except:
		_die(USAGE_MSG)
	mac_address_suffix = ((sid & 3) << 16) | ((sid >> 2) & 0xFFFF)
	dwMajorVersion = (sid >> 18) & 7
	dwMinorVersion = (sid >> 21) & 7
	wServicePackMajor = (sid >> 24) & 7
	wServicePackMinor = (sid >> 27) & 7
	wProductType = (sid >> 30) & 7
	print "MAC Address (18 LSb): 0x%06x" % mac_address_suffix
	os_name_list = guess_os(dwMajorVersion, dwMinorVersion, wProductType)
	if len(os_name_list) > 0:
		print_os_name_list(os_name_list)
	else:
		print "OS Major Version: 0x%08x" % dwMajorVersion
		print "OS Minor Version : 0x%08x" % dwMinorVersion
		print "OS Product Type: %s" % PRODUCT_TYPE_MAP[wProductType-1]
	print "OS Service Pack: %d.%d" % (wServicePackMajor, wServicePackMinor)

def print_os_name_list(os_name_list):
	if len(os_name_list) < 1:
		return
	if len(os_name_list) == 1:
		os_name = os_name_list[0]
	else:
		os_name = ' (or) '.join(os_name_list)
	print "OS: %s" % os_name

def guess_os(dwMajorVersion, dwMinorVersion, wProductType):
	os_name_list = []
	for os in OS_TABLE:
		if os[1] == dwMajorVersion and os[2] == dwMinorVersion and os[3](wProductType):
			os_name_list.append(os[0])
	return os_name_list

def _die(msg, exitcode = -1):
	print >> sys.stderr, "Error: %s" % msg
	exit(exitcode)

if __name__ == "__main__":
	main(sys.argv[1:])
