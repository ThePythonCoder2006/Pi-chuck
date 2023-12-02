#include <stdint.h>
#include <assert.h>

#ifndef __DAI_H__
#define __DAI_H__

#define DAI_ERROR "[ERROR] "
#define DAI_INFO "[INFO] "
#define DAI_TODO "[TODO] "

#define BILLION (1000ULL * 1000ULL * 1000ULL)

#ifdef __DAI_USE_GMP__

#include "DAI_gmp_wrapper.h"

#else // __DAI_USE_GMP__

#include "DAI_custom.h"

#endif

typedef DAI_t *DAI_ptr;

DAI_ret_t DAI_clean(DAI_ptr rop);
DAI_ret_t DAI_correct_flags(DAI_t rop);

DAI_ret_t DAI_set_ui(DAI_t rop, uint64_t op);
DAI_ret_t DAI_set(DAI_t rop, DAI_t op);

DAI_ret_t DAI_add(DAI_t rop, DAI_t op1, DAI_t op2);
DAI_ret_t DAI_sub(DAI_t rop, DAI_t op1, DAI_t op2);
DAI_ret_t DAI_mult(DAI_t rop, DAI_t op1, DAI_t op2);
DAI_ret_t DAI_mult_smol_int(DAI_t rop, DAI_t op1, uint32_t op2);

DAI_ret_t DAI_print(DAI_t op);

#endif // __DAI_H__
