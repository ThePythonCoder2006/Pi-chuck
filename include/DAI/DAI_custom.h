#ifndef __DAI_USE_GMP__

#include <stdint.h>
#include <inttypes.h>

#ifndef __DAI_CUSTOM__
#define __DAI_CUSTOM__

typedef uint32_t DAI_dec_unit_t;
typedef uint64_t DAI_prec_t;
#define DAI_PREC_MAX (UINT64_MAX)

typedef struct DAI_s
{
  uint8_t flags;
  DAI_dec_unit_t *data;
  DAI_prec_t prec;
} DAI_;

typedef DAI_ *DAI_t;
typedef DAI_t *DAI_ptr;

// error handling macros
typedef enum DAI_ret_e
{
  DAI_RET_OK,
  DAI_RET_PREC_ERROR,
  DAI_RET_MEM_ERROR,
  DAI_RET_TODO,
  DAI_RET_TOT,
} DAI_ret_t;

#define DAI_ERRORMSG_SIZE (256)
typedef char DAI_errormsg[DAI_ERRORMSG_SIZE];
extern DAI_errormsg DAI_errormsgs[DAI_RET_TOT + 1];

/*
 * fn is a function call whose return value will be checked
 * a variable `DAI_CHECK_ret` of type DAI_ret_t must have been defined previously
 */
#define DAI_CHECK_RET_VALUE(fn, ...)                                                                                         \
  if ((DAI_CHECK_ret = fn) != DAI_RET_OK)                                                                                    \
  {                                                                                                                          \
    fprintf(stderr, DAI_ERROR "%s:%u:%s : " __VA_ARGS__ "%s\n", __FILE__, __LINE__, __func__, *DAI_strerror(DAI_CHECK_ret)); \
    return DAI_CHECK_ret;                                                                                                    \
  }

extern DAI_ret_t DAI_CHECK_ret;

#define DAI_DEC_UNIT_MAX BILLION

// flags

#define DAI_FLAGS_NONE (0x0)
#define DAI_FLAGS_ZERO (0x1)
#define DAI_FLAGS_NEGA (0x2)

#define DAI_IS_ZERO(op) ((op->flags | DAI_FLAGS_ZERO) == op->flags)

#define DAI_GET_PREC(op) (op->prec)

#define DAI_INIT(rop, prec, ...) DAI_init((rop), (prec))
DAI_ret_t DAI_init(DAI_ptr rop, uint64_t prec);

DAI_errormsg *DAI_strerror(DAI_ret_t op);

#endif // __DAI_CUSTOM__

#endif // __DAI_USE_GMP__