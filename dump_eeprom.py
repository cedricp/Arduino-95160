#!/usr/bin/python3

# Usage dump_eeprom.py [port] [file]

import serial, sys


if len(sys.argv) != 3:
  print("Usage : dump_eeprom.py [port] [file]")
  exit(0)

device = sys.argv[1]
filename = sys.argv[2]


port = serial.Serial(device, baudrate=115200, parity=serial.PARITY_NONE, rtscts=0,
bytesize = 8)

# Reset Arduino
port.setDTR(False) # Drop DTR
port.setDTR(True)  # UP the DTR back

print(port.readline())
port.write(b"read 0 7FF\n")

f = open(filename, "wb")

line = port.readline()
line = port.readline()
print(line)
bytelist=line.decode("utf-8").strip().split(' ')
print (bytelist)
for h in bytelist:
  b = bytes.fromhex(h.zfill(2))
  f.write(b)
  
f.close()
