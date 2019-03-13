#include <stdint.h>
#include <string.h>

#define BMEMSIZE 16777216
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

void run(uint8_t *prog, size_t progsize) {
    memcpy(ubmem, prog, progsize);

    int64_t ds[32];
    int64_t rs[32];

    int64_t *uds = (uint64_t *)ds;
    double *fds = (double *)ds;

    int64_t pc, dsc, rsc, tmp;

    static void *dispatch[] = {
        &&halt,
        &&pass,
        &&litb,
        &&litub,
        &&litw,
        &&lituw,
        &&litd,
        &&litud,
        &&litq,
        &&addpc,
        &&jump,
        &&retjump,
        &&cjump,
        &&cretjump,
        &&call,
        &&retcall,
        &&ccall,
        &&cretcall,
        &&dget,
        &&dset,
        &&dup,
        &&drop,
        &&nip,
        &&swap,
        &&rollu,
        &&rolld,
        &&loadb,
        &&loadub,
        &&loadw,
        &&loaduw,
        &&loadd,
        &&loadud,
        &&loadq,
        &&storeb,
        &&storew,
        &&stored,
        &&storeq,
        &&not,
        &&and,
        &&or,
        &&xor,
        &&add,
        &&sub,
        &&mul,
        &&div,
        &&udiv,
        &&quo,
        &&uquo,
        &&rem,
        &&urem,
        &&shl,
        &&shr,
        &&ushr,
        &&cmp,
        &&ucmp,
        &&eq,
        &&ne,
        &&lt,
        &&ult,
        &&le,
        &&ule,
        &&gt,
        &&ugt,
        &&ge,
        &&uge,
        &&fadd,
        &&fsub,
        &&fmul,
        &&fdiv,
        &&fquo,
        &&frem,
        &&fint,
        &&feq,
        &&fne,
        &&flt,
        &&fle,
        &&fgt,
        &&fge,
    };

    #define DISPATCH(pc) goto *dispatch[ubmem[pc]]

    DISPATCH(pc);

    halt:
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
    dcall:
        rs[rsc++] = pc + 1;
        DISPATCH(pc = ds[--dsc]);
    rcall:
        tmp = rs[rsc - 1];
        rs[rsc - 1] = pc + 1;
        DISPATCH(pc = tmp);
    dget:
        ds[dsc - 1] = ds[dsc - ds[dsc - 1] - 1];
        DISPATCH(++pc);
    dset:
        ds[dsc - ds[dsc - 1] - 2] = ds[dsc - 2];
        DISPATCH(++pc);
    add:
        uds[dsc - 1] += uds[--dsc];
        DISPATCH(++pc);
    sub:
        uds[dsc - 1] -= uds[--dsc];
        DISPATCH(++pc);
    neg:
        uds[dsc - 1] = -uds[dsc - 1];
        DISPATCH(++pc);
    mul:
        uds[dsc - 1] *= uds[--dsc];
        DISPATCH(++pc);
    div:
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
    fadd:
        fds[dsc - 1] += fds[--dsc];
        DISPATCH(++pc);
    fsub:
        fds[dsc - 1] -= fds[--dsc];
        DISPATCH(++pc);
    fmul:
        fds[dsc - 1] -= fds[--dsc];
        DISPATCH(++pc);
    fdiv:
        fds[dsc - 1] /= fds[--dsc];
        DISPATCH(++pc);
}
