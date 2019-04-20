import sys
read = str(sys.argv[1])
write =  str(sys.argv[2])
r = open(read,"r")
w = open(write,"w")
w.write("{")
size = 0
while True:
    byte = r.read(1)
    if byte == '':
        break
    size = size + 1
    w.write("0x"+bytes(byte).hex()+", ")
w.write("};")
print("Size:", size-16)
r.close()
w.close()

