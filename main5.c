#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BMEMSIZE 16777216ll
#define WMEMSIZE BMEMSIZE / 2
#define DMEMSIZE WMEMSIZE / 2
#define QMEMSIZE DMEMSIZE / 2

static int8_t bmem[BMEMSIZE];
static int16_t *wmem = (int16_t *)bmem;
static int32_t *dmem = (int32_t *)bmem;
static int64_t *qmem = (int64_t *)bmem;

static uint8_t *ubmem = (uint8_t *)bmem;
static uint16_t *uwmem = (uint16_t *)bmem;
static uint32_t *udmem = (uint32_t *)bmem;
static uint64_t *uqmem = (uint64_t *)bmem;

void run() {
    int64_t ds[32];
    int64_t rs[32];

    uint64_t *uds = (uint64_t *)ds;
    double *fds = (double *)ds;

    int64_t pc = 0, dsc = 0, rsc = 0, tmp;

    static void *dispatch[] = {
        &&halt, /* 00 */
        &&pass, /* 01 */
        &&litb, /* 02 */
        &&litub, /* 03 */
        &&litw, /* 04 */
        &&lituw, /* 05 */
        &&litd, /* 06 */
        &&litud, /* 07 */
        &&litq, /* 08 */
        &&djump, /* 09 */
        &&rjump, /* 0A */
        &&cdjump, /* 0B */
        &&crjump, /* 0C */
        &&dcall, /* 0D */
        &&rcall, /* 0E */
        &&cdcall, /* 0F */
        &&crcall, /* 10 */
        &&addpc, /* 11 */
        &&dget, /* 12 */
        &&dset, /* 13 */
        &&fall, /* 14 */
        &&dup, /* 15 */
        &&over, /* 16 */
        &&drop, /* 17 */
        &&nip, /* 18 */
        &&tuck, /* 19 */
        &&swap, /* 1A */
        &&rollu, /* 1B */
        &&rolld, /* 1C */
        &&r_d, /* 1D */
        &&d_r, /* 1E */
        &&loadb, /* 1F */
        &&loadub, /* 20 */
        &&loadw, /* 21 */
        &&loaduw, /* 22 */
        &&loadd, /* 23 */
        &&loadud, /* 24 */
        &&loadq, /* 25 */
        &&storeb, /* 26 */
        &&storew, /* 27 */
        &&stored, /* 28 */
        &&storeq, /* 29 */
        &&not, /* 2A */
        &&and, /* 2B */
        &&or, /* 2C */
        &&xor, /* 2D */
        &&inc, /* 2E */
        &&add, /* 2F */
        &&dec, /* 30 */
        &&sub, /* 31 */
        &&neg, /* 32 */
        &&abs_, /* 33 */
        &&mul, /* 34 */
        &&div_, /* 35 */
        &&udiv, /* 36 */
        &&quo, /* 37 */
        &&uquo, /* 38 */
        &&rem, /* 39 */
        &&urem, /* 3A */
        &&shl, /* 3B */
        &&shr, /* 3C */
        &&ushr, /* 3D */
        &&eq, /* 3E */
        &&ne, /* 3F */
        &&lt, /* 40 */
        &&ult, /* 41 */
        &&le, /* 42 */
        &&ule, /* 43 */
        &&gt, /* 44 */
        &&ugt, /* 45 */
        &&ge, /* 46 */
        &&uge, /* 47 */
        &&fadd, /* 48 */
        &&fsub, /* 49 */
        &&fneg, /* 4A */
        &&fabs_, /* 4B */
        &&fmul, /* 4C */
        &&fdiv, /* 4D */
        &&flt, /* 4E */
        &&fle, /* 4F */
        &&fgt, /* 50 */
        &&fge, /* 51 */
        &&fint, /* 52 */
        &&float_, /* 53 */
        &&ufloat, /* 54 */
    };

    #define DISPATCH(PC) ({\
        int64_t pc_ = (PC);\
        printf("pc %ld, inst %u, dsc %ld, rsc %ld ; ", pc_, ubmem[pc_], dsc, rsc);\
        for (int i = 0; i < dsc; ++i) printf("%ld ", ds[i]);\
        printf("; ");\
        for (int i = 0; i < rsc; ++i) printf("%ld ", rs[i]);\
        printf("\n");\
        goto *dispatch[ubmem[pc_]];\
    })

    DISPATCH(pc);

    halt:
        printf("%ld\n", ds[dsc - 1]);
        return;
    pass:
        DISPATCH(++pc);
    litb:
        ds[dsc++] = bmem[++pc];
        DISPATCH(++pc);
    litub:
        uds[dsc++] = ubmem[++pc];
        DISPATCH(++pc);
    litw:
        ds[dsc++] = wmem[++pc >> 1];
        DISPATCH(pc += 2);
    lituw:
        uds[dsc++] = uwmem[++pc];
        DISPATCH(pc += 2);
    litd:
        ds[dsc++] = dmem[++pc >> 2];
        DISPATCH(pc += 4);
    litud:
        uds[dsc++] = udmem[++pc >> 2];
        DISPATCH(pc += 4);
    litq:
        ds[dsc++] = qmem[++pc >> 3];
        DISPATCH(pc += 8);
    djump:
        DISPATCH(pc = ds[--dsc]);
    rjump:
        DISPATCH(pc = rs[--rsc]);
    cdjump:
        DISPATCH(pc = ds[dsc -= 2] ? pc + 1 : ds[dsc + 1]);
    crjump:
        DISPATCH(pc = ds[--dsc] ? pc + 1 : rs[--rsc]);
    dcall:
        rs[rsc++] = pc + 1;
        DISPATCH(pc = ds[--dsc]);
    rcall:
        tmp = rs[rsc - 1];
        rs[rsc - 1] = pc + 1;
        DISPATCH(pc = tmp);
    cdcall:
        if (ds[dsc -= 2]) ++pc;
        else {
            rs[rsc++] = pc + 1;
            DISPATCH(pc = ds[dsc + 1]);
        }
    crcall:
        if (ds[--dsc]) ++pc;
        else {
            tmp = rs[rsc - 1];
            rs[rsc - 1] = pc + 1;
            DISPATCH(pc = tmp);
        }
    addpc:
        ds[dsc - 1] += pc;
        DISPATCH(++pc);
    dget:
        ds[dsc - 1] = ds[dsc - ds[dsc - 1] - 1];
        DISPATCH(++pc);
    dset:
        ds[dsc - ds[dsc - 1] - 2] = ds[dsc - 2];
        DISPATCH(++pc);
    fall:
        dsc -= ds[dsc - 1] - 1;
        DISPATCH(++pc);
    dup:
        ds[dsc++] = ds[dsc - 1];
        DISPATCH(++pc);
    over:
        ds[dsc++] = ds[dsc - 2];
        DISPATCH(++pc);
    drop:
        --dsc;
        DISPATCH(++pc);
    nip:
        ds[dsc - 1] = ds[--dsc];
        DISPATCH(++pc);
    tuck:
        tmp = ds[dsc - 1];
        ds[dsc - 1] = ds[dsc - 2];
        ds[dsc - 2] = tmp;
        ds[dsc++] = tmp;
        DISPATCH(++pc);
    swap:
        tmp = ds[dsc - 1];
        ds[dsc - 1] = ds[dsc - 2];
        ds[dsc - 2] = tmp;
        DISPATCH(++pc);
    rollu:
        tmp = ds[dsc - 1];
        ds[dsc - 1] = ds[dsc - 2];
        ds[dsc - 2] = ds[dsc - 3];
        ds[dsc - 3] = tmp;
        DISPATCH(++pc);
    rolld:
        tmp = ds[dsc - 3];
        ds[dsc - 3] = ds[dsc - 2];
        ds[dsc - 2] = ds[dsc - 1];
        ds[dsc - 1] = tmp;
        DISPATCH(++pc); 
    r_d:
        ds[dsc++] = rs[--rsc];
        DISPATCH(++pc);
    d_r:
        rs[rsc++] = ds[--dsc];
        DISPATCH(++pc);
    loadb:
        ds[dsc - 1] = bmem[ds[dsc - 1]];
        DISPATCH(++pc);
    loadub:
        uds[dsc - 1] = ubmem[ds[dsc - 1]];
        DISPATCH(++pc);
    loadw:
        ds[dsc - 1] = wmem[ds[dsc - 1]];
        DISPATCH(++pc);
    loaduw:
        uds[dsc - 1] = uwmem[ds[dsc - 1]];
        DISPATCH(++pc);
    loadd:
        ds[dsc - 1] = dmem[ds[dsc - 1]];
        DISPATCH(++pc);
    loadud:
        ds[dsc - 1] = udmem[ds[dsc - 1]];
        DISPATCH(++pc);
    loadq:
        ds[dsc - 1] = qmem[ds[dsc - 1]];
        DISPATCH(++pc);
    storeb:
        bmem[ds[dsc - 1]] = ds[dsc - 2];
        dsc -= 2;
        DISPATCH(++pc);
    storew:
        wmem[ds[dsc - 1]] = ds[dsc - 2];
        dsc -= 2;
        DISPATCH(++pc);
    stored:
        dmem[ds[dsc - 1]] = ds[dsc - 2];
        dsc -= 2;
        DISPATCH(++pc);
    storeq:
        qmem[ds[dsc - 1]] = ds[dsc - 2];
        dsc -= 2;
        DISPATCH(++pc);
    not:
        ds[dsc - 1] = ~ds[dsc - 1];
        DISPATCH(++pc);
    and:
        ds[dsc - 1] &= ds[--dsc];
        DISPATCH(++pc);
    or:
        ds[dsc - 1] |= ds[--dsc];
        DISPATCH(++pc);
    xor:
        ds[dsc - 1] ^= ds[--dsc];
        DISPATCH(++pc);
    inc:
        ++ds[dsc - 1];
        DISPATCH(++pc);
    add:
        uds[dsc - 1] += uds[--dsc];
        DISPATCH(++pc);
    dec:
        --ds[dsc - 1];
        DISPATCH(++pc);
    sub:
        uds[dsc - 1] -= uds[--dsc];
        DISPATCH(++pc);
    neg:
        uds[dsc - 1] = -uds[dsc - 1];
        DISPATCH(++pc);
    abs_:
        uds[dsc - 1] = abs(uds[dsc - 1]);
        DISPATCH(++pc);
    mul:
        uds[dsc - 1] *= uds[--dsc];
        DISPATCH(++pc);
    div_:
        tmp = ds[dsc - 2] % ds[dsc - 1];
        ds[dsc - 2] /= ds[dsc - 1];
        ds[dsc - 1] = tmp;
        DISPATCH(++pc);
    udiv:
        tmp = uds[dsc - 2] % uds[dsc - 1];
        uds[dsc - 2] /= uds[dsc - 1];
        uds[dsc - 1] = tmp;
        DISPATCH(++pc);
    quo:
        ds[dsc - 1] /= ds[--dsc];
        DISPATCH(++pc);
    uquo:
        uds[dsc - 1] /= uds[--dsc];
        DISPATCH(++pc);
    rem:
        ds[dsc - 1] %= ds[--dsc];
        DISPATCH(++pc);
    urem:
        uds[dsc - 1] %= uds[--dsc];
        DISPATCH(++pc);
    shl:
        uds[dsc - 1] <<= uds[--dsc];
        DISPATCH(++pc);
    shr:
        ds[dsc - 1] >>= ds[--dsc];
        DISPATCH(++pc);
    ushr:
        uds[dsc - 1] >>= uds[--dsc];
        DISPATCH(++pc);
    eq:
        ds[dsc - 2] = ds[dsc - 1] == ds[dsc - 2] ? -1 : 0;
        --dsc;
        DISPATCH(++pc);
    ne:
        ds[dsc - 2] = ds[dsc - 1] != ds[dsc - 2] ? -1 : 0;
        --dsc;
        DISPATCH(++pc);
    lt:
        ds[dsc - 2] = ds[dsc - 2] < ds[dsc - 1] ? -1 : 0;
        --dsc;
        DISPATCH(++pc);
    ult:
        ds[dsc - 2] = uds[dsc - 2] < uds[dsc - 1] ? -1 : 0;
        --dsc;
        DISPATCH(++pc);
    le:
        ds[dsc - 2] = ds[dsc - 2] <= ds[dsc - 1] ? -1 : 0;
        --dsc;
        DISPATCH(++pc);
    ule:
        ds[dsc - 2] = uds[dsc - 2] <= uds[dsc - 1] ? -1 : 0;
        --dsc;
        DISPATCH(++pc);
    gt:
        ds[dsc - 2] = ds[dsc - 2] > ds[dsc - 1] ? -1 : 0;
        --dsc;
        DISPATCH(++pc);
    ugt:
        ds[dsc - 2] = uds[dsc - 2] > uds[dsc - 1] ? -1 : 0;
        --dsc;
        DISPATCH(++pc);
    ge:
        ds[dsc - 2] = ds[dsc - 2] >= ds[dsc - 1] ? -1 : 0;
        --dsc;
        DISPATCH(++pc);
    uge:
        ds[dsc - 2] = uds[dsc - 2] >= uds[dsc - 1] ? -1 : 0;
        --dsc;
        DISPATCH(++pc);
    fadd:
        fds[dsc - 1] += fds[--dsc];
        DISPATCH(++pc);
    fsub:
        fds[dsc - 1] -= fds[--dsc];
        DISPATCH(++pc);
    fneg:
        fds[dsc - 1] = -fds[dsc - 1];
        DISPATCH(++pc);
    fabs_:
        fds[dsc - 1] = fabs(fds[dsc - 1]);
        DISPATCH(++pc);
    fmul:
        fds[dsc - 1] -= fds[--dsc];
        DISPATCH(++pc);
    fdiv:
        fds[dsc - 1] /= fds[--dsc];
        DISPATCH(++pc);
    flt:
        fds[dsc - 2] = fds[dsc - 2] < fds[dsc -1];
        --dsc;
        DISPATCH(++pc);
    fle:
        fds[dsc - 2] = fds[dsc - 2] <= fds[dsc -1];
        --dsc;
        DISPATCH(++pc);
    fgt:
        fds[dsc - 2] = fds[dsc - 2] > fds[dsc -1];
        --dsc;
        DISPATCH(++pc);
    fge:
        fds[dsc - 2] = fds[dsc - 2] >= fds[dsc -1];
        --dsc;
        DISPATCH(++pc);
    fint:
        ds[dsc - 1] = (int64_t)fds[dsc - 1];
        DISPATCH(++pc);
    float_:
        fds[dsc - 1] = (double)uds[dsc - 1];
        DISPATCH(++pc);
    ufloat:
        fds[dsc - 1] = (double)ds[dsc - 1];
        DISPATCH(++pc);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Invalid number of arguments: %d (exactly 1 required)\n", argc - 1);
        goto fail;
    }

    FILE *file = fopen(argv[1], "rb");

    if (!file) {
        perror("Failed to open file");
        goto fail;
    }

    fread(bmem, 1, BMEMSIZE, file);

    if (ferror(file)) {
        perror("Error reading file");
        goto fail;
    }

    if (!feof(file)) {
        fprintf(stderr, "Program too large, must be <= %lld bytes\n", BMEMSIZE);
        goto fail;
    }

    run();

    fail:
        return EXIT_FAILURE;
}

/*
factorial:
    dup litb 2 > crjump
    dup litb 1 - *
    :factorial djump
*/