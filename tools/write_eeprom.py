#!/usr/bin/python3

# You need to install pyserial

import serial, sys

if len(sys.argv) != 3:
  print("Usage : write_eeprom.py [port] [file]")
  exit(0)

device = sys.argv[1]
filename = sys.argv[2]

port = serial.Serial(device, baudrate=115200, parity=serial.PARITY_NONE, rtscts=0,
bytesize = 8)

# Reset Arduino
port.setDTR(False) # Drop DTR
port.setDTR(True)  # UP the DTR back

print(port)
print(port.readline().decode("utf-8"))

f = open(filename, "rb")

for i in range(0, 0x800):
  b = f.read(1)
  data = hex(i)[2:] + " " + b.hex()
  sys.stdout.write("Writing : " + data + "\r")
  port.write(b"write " + str.encode(data) + b"\n")
  ret = ""
  while(1):
    char = port.read(1).decode("utf-8")
    ret += char
    if char == ">":
      break
  if "ERROR" in ret:
    print("Error detected : " + ret)
    break
    
f.close()
print("EEPROM write done")

