#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Invalid number of arguments: %d (exactly 1 required)", argc - 1);
        exit(EXIT_FAILURE);
    }

    uint8_t *mem = malloc(65536);

    if (!mem) {
        perror("Failed to allocate program memory");
        exit(EXIT_FAILURE);
    }

    const char *filename = argv[1];
    FILE * file = fopen(filename, "rb");

    if (!file) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    fread(mem, 1, 65536, file);

    if (ferror(file)) {
        perror("Error reading file");
        exit(EXIT_FAILURE);
    }

    if (!feof(file)) {
        fprintf(stderr, "File too large (<= 64 KB required)");
        exit(EXIT_FAILURE);
    }

    uint16_t *rs = malloc(512);

    if (!rs) {
        perror("Failed to allocate the return stack");
        exit(EXIT_FAILURE);
    }

    uint16_t *ds = malloc(512);

    if (!ds) {
        perror("Failed to allocate the data stack");
        exit(EXIT_FAILURE);
    }

    uint16_t pc = 0;
    uint8_t rsc = 0;
    uint8_t dsc = 0;

    for (;;) {
        switch (mem[pc]) {
            case 0: /* jump */
                pc = mem[pc + 1] << 8 | mem[pc + 2];
                break;
            case 1: /* relative jump */
                pc += mem[pc + 1];
                break;
            case 3: /* indirect jump */
                pc = rs[--rsc];
                break;
            case 4: /* call */
                rs[rsc++] = pc + 3;
                pc = mem[pc + 1] << 8 | mem[pc + 2];
                break;
            case 5: /* relative call */
                rs[rsc++] = pc + 2;
                pc += mem[pc + 1];
                break;
            case 6: /* indirect call */
                rs[rsc++] = pc + 1;
                pc = rs[--rsc];
                break;
            case 7: /* branch */
                pc = ds[--dsc] ? pc + 3 : mem[pc + 1] << 8 | mem[pc + 2];
                break;
            case 8: /* relative branch with byte offset */
                pc += ds[--dsc] ? 3 : mem[pc + 1];
                break;
            case 9: /* indirect branch */
                pc = ds[--dsc] ? pc + 1 : rs[--rsc];
                break;
            case 10: /* return stack -> data stack */
                ds[dsc++] = rs[--rsc];
                ++pc;
                break;
            case 11: /* data stack -> return stack */
                rs[rsc++] = ds[--dsc];
                ++pc;
                break;
            case 12: /* push byte */
                ds[dsc++] = mem[++pc];
                ++pc;
                break;
            case 13: /* push */
                ds[dsc++] = mem[pc + 1] << 8 | mem[pc + 2];
                pc += 3;
                break;
            case 14: /* load byte */
                ds[dsc++] = mem[mem[pc + 1] << 8 | mem[pc + 2]];
                pc += 3;
                break;
            case 15: /* load signed byte */
                (int16_t *)&ds[dsc++] = mem[mem[pc + 1] << 8 | mem[pc + 2]];
                pc += 3;
                break;
            case 15: { /* load */
                uint16_t addr = mem[pc + 1] << 8 | mem[pc + 2];
                ds[dsc++] = mem[addr] << 8 | mem[addr + 1];
                pc += 3;
                } break;
            case 16: /* indirect load byte */
                ds[dsc - 1] = mem[ds[dsc - 1]];
                ++pc;
                break;
            case 17: /* indirect load signed byte */
                (int16_t *)ds[dsc - 1] = mem[ds[dsc - 1]];
                ++pc;
                break;
            case 18: { /* indirect load */
                uint16_t addr = ds[dsc - 1];
                ds[dsc - 1] = mem[addr] << 8 | mem[addr + 1];
                } break;
        }
    }
}