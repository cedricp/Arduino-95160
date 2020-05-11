#!/usr/bin/python3

import serial

port = serial.Serial("/dev/ttyACM0", baudrate=115200, parity=serial.PARITY_NONE, rtscts=0,
bytesize = 8)
print(port)
print(port.readline())
port.write(b"read 0 7FF\n")

f = open("myS3000.bin", "wb")

line = port.readline()
line = port.readline()
print(line)
bytelist=line.decode("utf-8").strip().split(' ')
print (bytelist)
for h in bytelist:
  b = bytes.fromhex(h.zfill(2))
  f.write(b)
  
f.close()
