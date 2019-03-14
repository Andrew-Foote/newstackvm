import sys

def binaryize(src):
    for token in src.split():
        value = int(token, 16)
        
        while value:
            yield value & 255
            value >>= 8

if __name__ == '__main__':
    argc = len(sys.argv) 

    if argc != 3:
        print(f'Invalid number of arguments: {argc} (2 required)', file=sys.stderr)
        sys.exit(-1)

    ifilename = sys.argv[1]
    ofilename = sys.argv[2]
    
    with open(ifilename, 'r') as f:
        src = f.read()

    bytes_ = bytearray(binaryize(src))

    with open(ofilename, 'wb') as f:
        f.write(bytes_)
