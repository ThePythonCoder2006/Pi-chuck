#include <stdint.h>
#include <assert.h>

#ifndef __DAI_H__
#define __DAI_H__

#define DAI_ERROR "[ERROR] "
#define DAI_INFO "[INFO] "
#define DAI_TODO "[TODO] "

typedef uint32_t DAI_dec_unit_t;
#define DAI_DEC_UNIT_MAX (1000ULL * 1000ULL * 1000ULL)
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

// flags

#define DAI_FLAGS_NONE (0x0)
#define DAI_FLAGS_ZERO (0x1)

#define DAI_IS_ZERO(op) ((op->flags | DAI_FLAGS_ZERO) == op->flags)

DAI_ret_t DAI_init(DAI_ptr rop, uint64_t prec);
DAI_ret_t DAI_clean(DAI_ptr rop);

DAI_ret_t DAI_set_ui(DAI_t rop, uint64_t op);
DAI_ret_t DAI_set(DAI_t rop, DAI_t op);

DAI_ret_t DAI_add(DAI_t rop, DAI_t op1, DAI_t op2);
DAI_ret_t DAI_mult(DAI_t rop, DAI_t op1, DAI_t op2);
DAI_ret_t DAI_mult_smol_int(DAI_t rop, DAI_t op1, DAI_dec_unit_t op2);

DAI_ret_t DAI_print(DAI_t op);

DAI_errormsg *DAI_strerror(DAI_ret_t op);

#endif // __DAI_H__
