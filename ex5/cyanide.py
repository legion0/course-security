import socket
import struct

UDP_IP = "192.168.146.139"
UDP_PORT = 1337
sock = socket.socket(socket.AF_INET, # Internet
                        socket.SOCK_DGRAM) # UDP

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

sock.bind((UDP_IP, UDP_PORT))

udp_init_start = '\xde\xaf\x01'
mysid = 55
myport = 31337
udp_resp_header = '\xde\xaf\x02'

s.bind((UDP_IP, myport))


while True:
    data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
    
    if not (len(data) == 7):
        continue
    bytes,sid = struct.unpack('!3si',data)
    print bytes, sid

    if not (bytes == '\xde\xaf\x01') or not (sid == mysid):
        continue

    #perhaps find actual servers port.
    
    format = '!3sihI'
    crc = (sid ^ 0xCAFEBABE) + 4 * (myport ^ 0x652)
    data = struct.pack(format,udp_resp_header,mysid,myport,crc)
    sock.sendto(data,addr)
    sock.close()
    
    s.listen(1)
    conn, addr = s.accept()
    print 'Connection address:', addr
    data = conn.recv(20)
    
    header , dlen = struct.unpack('!3sh',data[:5])
    path = struct.unpack('!{0}s'.format(dlen),data[5:])[0]

    print header==udp_init_start,dlen,path

    data = struct.pack('!3sh',udp_resp_header,1)

    fname = 'hello.poop'
    file = struct.pack('!bih{0}s'.format(len(fname)),0x40,0,len(fname),fname)
    conn.send(data+file)

    conn.close()
    s.close()
    break


