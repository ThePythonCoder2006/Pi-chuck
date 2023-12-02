#ifdef __DAI_USE_GMP__

#ifndef __DAI_GMP_WRAPPER__
#define __DAI_GMP_WRAPPER__

#include <gmp.h>

#define DAI_RET_OK

typedef void DAI_ret_t;
typedef mpz_t DAI_t;

#define DAI_GET_PREC(...)

#define DAI_INIT(rop, ...) mpz_init((*rop))

#define DAI_CHECK_RET_VALUE(fn, ...) fn

#endif //__DAI_GMP_WRAPPER__

#endif // __DAI_USE_GMP__