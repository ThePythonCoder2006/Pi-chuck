#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <assert.h>

#include "DAI.h"

#ifdef __DAI_USE_GMP__

DAI_ret_t DAI_clean(DAI_ptr rop) { mpz_clear(*rop); }
DAI_ret_t DAI_set_ui(DAI_t rop, uint64_t op) { mpz_set_ui(rop, op); }
DAI_ret_t DAI_mult(DAI_t rop, DAI_t op1, DAI_t op2) { mpz_mul(rop, op1, op2); }
DAI_ret_t DAI_mult_smol_int(DAI_t rop, DAI_t op1, uint32_t op2) { mpz_mul_ui(rop, op1, op2); }
DAI_ret_t DAI_sub(DAI_t rop, DAI_t op1, DAI_t op2) { mpz_sub(rop, op1, op2); }
DAI_ret_t DAI_add(DAI_t rop, DAI_t op1, DAI_t op2) { mpz_add(rop, op1, op2); }
DAI_ret_t DAI_print(DAI_t op) { gmp_printf("%Zd", op); }

#endif // __DAI_USE_GMP__