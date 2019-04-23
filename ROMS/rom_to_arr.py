import sys
read = str(sys.argv[1])
write =  str(sys.argv[2])
r = open(read,"r", encoding = "ISO-8859-1")
w = open(write,"w")
w.write("{")
size = 0
while True:
	byte = r.read(1)
	if byte == '':
		break
	size = size + 1
	if size % 32 == 0:
		w.write("0x"+bytes(byte,"ISO-8859-1").hex()+", \n")
	else: 
		w.write("0x"+bytes(byte,"ISO-8859-1").hex()+", ")
w.write("};")
print("Size:", size-16)
r.close()
w.close()
 
