from pathlib import Path
import sys

OPCODES = {
    'halt': 0x00,
    'pass': 0x01,
    'litb': 0x02,
    'litub': 0x03,
    'litw': 0x04,
    'lituw': 0x05,
    'litd': 0x06,
    'litud': 0x07,
    'litq': 0x08,
    'djump': 0x09,
    'rjump': 0x0A,
    'cdjump': 0x0B,
    'crjump': 0x0C,
    'dcall': 0x0D,
    'rcall': 0x0E,
    'cdcall': 0x0F,
    'crcall': 0x10,
    'addpc': 0x11,
    'dget': 0x12,
    'dset': 0x13,
    'fall': 0x14,
    'dup': 0x15,
    'over': 0x16,
    'drop': 0x17,
    'nip': 0x18,
    'tuck': 0x19,
    'swap': 0x1A,
    'rollu': 0x1B,
    'rolld': 0x1C,
    'r_d': 0x1D,
    'd_r': 0x1E,
    'loadb': 0x1F,
    'loadub': 0x20,
    'loadw': 0x21,
    'loaduw': 0x22,
    'loadd': 0x23,
    'loadud': 0x24,
    'loadq': 0x25,
    'storeb': 0x26,
    'storew': 0x27,
    'stored': 0x28,
    'storeq': 0x29,
    'not': 0x2A,
    'and': 0x2B,
    'or': 0x2C,
    'xor': 0x2D,
    'inc': 0x2E,
    'add': 0x2F,
    'dec': 0x30,
    'sub': 0x31,
    'neg': 0x32,
    'abs_': 0x33,
    'mul': 0x34,
    'div_': 0x35,
    'udiv': 0x36,
    'quo': 0x37,
    'uquo': 0x38,
    'rem': 0x39,
    'urem': 0x3A,
    'shl': 0x3B,
    'shr': 0x3C,
    'ushr': 0x3D,
    'eq': 0x3E,
    'ne': 0x3F,
    'lt': 0x40,
    'ult': 0x41,
    'le': 0x42,
    'ule': 0x43,
    'gt': 0x44,
    'ugt': 0x45,
    'ge': 0x46,
    'uge': 0x47,
    'fadd': 0x48,
    'fsub': 0x49,
    'fneg': 0x4A,
    'fabs_': 0x4B,
    'fmul': 0x4C,
    'fdiv': 0x4D,
    'feq': 0x4E,
    'fne': 0x4F,
    'flt': 0x50,
    'fle': 0x51,
    'fgt': 0x52,
    'fge': 0x53,
    'fint': 0x54,
    'float_': 0x55,
    'ufloat': 0x57,
}

def parse_int_token(value):
    if number < 0:
        if number >= -128:
            yield OPCODES['litb']
            yield number + 256
        elif number >= -65536:
            yield OPCODES['litw']
            yield from int.to_bytes(number + 65536, sys.byteorder, signed=True)
        elif number >= -2147483648:
            yield OPCODES['litd']
            yield from int.to_bytes(number + 2147483648, sys.byteorder, signed=True)
        elif number >= -9223372036854775808:
            yield OPCODES['litq']
            yield from int.to_bytes(number + 9223372036854775808, sys.byteorder, signed=True)
        else:
            raise ValueError(f'integer literal out of range: {number}')
    else:
        if number < 256:
            yield OPCODES['litub']
            yield number
        elif number < 65536:
            yield OPCODES['lituw']
            yield from int.to_bytes(number, sys.byteorder)
        elif number < 4294967296  :
            yield OPCODES['litud']
            yield from int.to_bytes(number, sys.byteorder)
        elif number < 18446744073709551616:
            yield OPCODES['litq']
            yield from int.to_bytes(number, sys.byteorder)
        else:
            raise ValueError(f'integer literal out of range: {number}')

def parse_float_token(value):
    

def assemble(src):
    for token in src.split():
        try:
            value = int(token)
        except ValueError:
            try:
                value = float(token)
            except ValueError:
                yield from parse_float_token(value)

            yield OPCODES[token]
        else:
            yield from parse_int_token(value)
            neg = num < 0
            bytes_ = []
            
            for _ in range(8):
                bytes_.append(num & 255)
                num >>= 8
            
            if num:
                raise ValueError(f'integer {num} is too large')

            opcode = math.log2(len(bytes_)) + 2
            opcode |= not neg * 8
            yield opcode
            yield from bytes_

if __name__ == '__main__':
    argc = len(sys.argv) 

    if not argc or argc > 2:
        print(f'Invalid number of arguments: {argc} (1 or 2 required)', file=sys.stderr)
        sys.exit(-1)

    ifilename = sys.argv[1]

    if argc == 2:
        ofilename = sys.argv[2]
    else:
        ofilename = Path(ofilename).stem + '.csmo'

    with open(ifilename, 'r') as f:
        src = f.read()

    program = assemble(src)

    with open(ofilename, 'wb') as f:
        f.write(program)
