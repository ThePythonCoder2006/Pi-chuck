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

// ------------ setup / cleanup functions --------------

// cleaning functions
DAI_ret_t DAI_clean(DAI_ptr rop);
DAI_ret_t DAI_correct_flags(DAI_t rop);

// setting functions
DAI_ret_t DAI_set_ui(DAI_t rop, uint64_t op);
DAI_ret_t DAI_set(DAI_t rop, DAI_t op);

// ------------ arithmetic functions -------------------

// addition functions
DAI_ret_t DAI_add_abs(DAI_t rop, DAI_t op1, DAI_t op2);
DAI_ret_t DAI_add(DAI_t rop, DAI_t op1, DAI_t op2);

DAI_ret_t DAI_negate(DAI_t rop, DAI_t op);

// substraction function
DAI_ret_t DAI_sub_abs(DAI_t rop, DAI_t op1, DAI_t op2);
DAI_ret_t DAI_sub(DAI_t rop, DAI_t op1, DAI_t op2);
DAI_ret_t DAI_smol_int_sub(DAI_t rop, DAI_dec_unit_t op1, DAI_t op2);

// multiplication functions
DAI_ret_t DAI_mul(DAI_t rop, DAI_t op1, DAI_t op2);
DAI_ret_t DAI_mul_smol_int(DAI_t rop, DAI_t op1, DAI_dec_unit_t op2);

// shift functions
DAI_ret_t DAI_srl(DAI_t rop, DAI_t op, DAI_prec_t n);
DAI_ret_t DAI_srl_dec(DAI_t rop, DAI_t op, DAI_prec_t n);

DAI_ret_t DAI_10_pow_n(DAI_t rop, uint64_t n);

// ------------- other functions ----------------------

// comparaison functions
DAI_ret_t DAI_cmp_abs(int8_t *rop, DAI_t A, DAI_t B);
DAI_ret_t DAI_cmp(int8_t *rop, DAI_t A, DAI_t B);

// printing functions
DAI_ret_t DAI_print(DAI_t op);
DAI_ret_t DAI_fprint(FILE *f, DAI_t op);

#endif // __DAI_H__
