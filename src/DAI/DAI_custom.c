#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <assert.h>

#include "DAI.h"

#ifndef __DAI_USE_GMP__

DAI_errormsg DAI_errormsgs[DAI_RET_TOT + 1] = {
    "nothing went wrong !",
    "the precision of the int given was not enough to perform the calculations that were asked !",
    "some memory could not be allocated !",
    "this function is not yet implemented !",
    "the error code given does not exist !",
};

DAI_ret_t DAI_CHECK_ret = DAI_RET_OK;

DAI_ret_t DAI_init(DAI_ptr rop, DAI_prec_t prec)
{
  *rop = malloc(sizeof(**rop));
  (*rop)->prec = prec;
  (*rop)->data = calloc(prec, sizeof(DAI_dec_unit_t));
  if ((*rop)->data == NULL)
    return DAI_RET_MEM_ERROR;

  (*rop)->flags = DAI_FLAGS_ZERO;
  return DAI_RET_OK;
}

DAI_ret_t DAI_clean(DAI_ptr rop)
{
  (*rop)->prec = 0;
  free((*rop)->data);
  (*rop)->data = NULL;
  (*rop)->flags = DAI_FLAGS_NONE;

  free(*rop);
  *rop = NULL;
  return DAI_RET_OK;
}

DAI_ret_t DAI_correct_flags(DAI_t rop)
{
  for (DAI_prec_t i = 0; i < rop->prec; ++i)
    if (rop->data[i] != 0)
    {
      rop->flags &= ~DAI_FLAGS_ZERO;
      return DAI_RET_OK;
    }

  rop->flags |= DAI_FLAGS_ZERO;
  return DAI_RET_OK;
}

DAI_ret_t DAI_set_zero(DAI_t rop)
{
  for (DAI_prec_t i = 0; i < rop->prec; ++i)
    rop->data[i] = 0;

  rop->flags = DAI_FLAGS_ZERO;

  return DAI_RET_OK;
}

DAI_ret_t DAI_set_ui(DAI_t rop, uint64_t op)
{
  DAI_set_zero(rop);

  if (op == 0)
    return DAI_RET_OK;
  rop->flags &= ~DAI_FLAGS_ZERO;

  if (rop->prec == 0)
    return DAI_RET_PREC_ERROR;

  if (rop->prec < 2 && op > DAI_DEC_UNIT_MAX) // rop->prec == 1
    return DAI_RET_PREC_ERROR;

  if (rop->prec < 3 && op > DAI_DEC_UNIT_MAX * DAI_DEC_UNIT_MAX) // rop->prec == 2
    return DAI_RET_PREC_ERROR;

  rop->data[0] = op % DAI_DEC_UNIT_MAX;
  rop->data[1] = (uint64_t)(op / DAI_DEC_UNIT_MAX) % DAI_DEC_UNIT_MAX;
  rop->data[2] = (uint64_t)((op / DAI_DEC_UNIT_MAX) / DAI_DEC_UNIT_MAX);

  return DAI_RET_OK;
}

DAI_ret_t DAI_set(DAI_t rop, DAI_t op)
{
  if (rop->prec != op->prec)
  {
    fprintf(stderr, DAI_TODO "copying ints with different precision is not (yet) implemented !!\n");
    return DAI_RET_TODO;
  }

  rop->flags = op->flags;
  rop->prec = op->prec;
  memcpy(rop->data, op->data, rop->prec * sizeof(rop->data[0]));

  return DAI_RET_OK;
}

typedef int8_t DAI_add_carry_t;

DAI_ret_t DAI_handle_carrys(DAI_t rop, DAI_add_carry_t *carrys, size_t carrys_size)
{
  DAI_add_carry_t *original_carrys = carrys;

  assert(carrys_size > 0);

  DAI_add_carry_t *carrys2 = calloc(carrys_size, sizeof(carrys[0]));
  if (carrys2 == NULL)
    return DAI_RET_MEM_ERROR;

  uint8_t quit = 0;
  while (!quit)
  {
    quit = 1;
    for (DAI_prec_t i = 1; i < carrys_size - 1; ++i)
    {
      if (carrys[i] < 0 && (signed int)rop->data[i] < carrys[i]) // negative carry from substraction needs to borrow from next one
      {
        rop->data[i] += DAI_DEC_UNIT_MAX + carrys[i];
        carrys2[i + 1] -= 1;
        if (i == carrys_size - 2) // number should be negative
        {
          printf("negative\n");
          return DAI_RET_OK;
        }
        quit = 0;
        continue;
      }

      rop->data[i] += carrys[i];
      if (rop->data[i] >= DAI_DEC_UNIT_MAX)
      {
        rop->data[i] -= DAI_DEC_UNIT_MAX;
        carrys2[i + 1] += 1;
        quit = 0;
      }
    }

    DAI_add_carry_t *tmp = carrys;
    carrys = carrys2;
    carrys2 = tmp;
    // memcpy(carrys, carrys2, carrys_size * sizeof(carrys[0]));
  }
  if (carrys2 == original_carrys)
    free(carrys);
  else
    free(carrys2);

  return DAI_RET_OK;
}

DAI_ret_t DAI_add(DAI_t rop, DAI_t op1, DAI_t op2)
{
  assert(rop != op1 && rop != op2);

  if (DAI_IS_ZERO(*op1) && DAI_IS_ZERO(*op2))
  {
    rop->flags |= DAI_FLAGS_ZERO;
    return DAI_RET_OK;
  }
  else
    rop->flags &= ~DAI_FLAGS_ZERO;

  DAI_prec_t min_prec = op1->prec < op2->prec ? op1->prec : op2->prec;

  if (rop->prec < min_prec)
    return DAI_RET_PREC_ERROR;

  DAI_add_carry_t *carrys = calloc(min_prec + 1, sizeof(DAI_add_carry_t));
  if (carrys == NULL)
    return DAI_RET_MEM_ERROR;

  for (DAI_prec_t i = 0; i < min_prec; ++i)
  {
    rop->data[i] = op1->data[i] + op2->data[i];
    if (rop->data[i] >= DAI_DEC_UNIT_MAX)
    {
      rop->data[i] -= DAI_DEC_UNIT_MAX;
      carrys[i + 1] = 1;
    }
  }

  if (carrys[min_prec] != 0)
  {
    if (rop->prec < min_prec + 1)
      return DAI_RET_PREC_ERROR;
  }

  DAI_CHECK_RET_VALUE(DAI_handle_carrys(rop, carrys, min_prec + 1));

  free(carrys);

  return DAI_RET_OK;
}

/*
 * computes rop = op1 * (1 000 000 000)^op1_shift + op2 * (1 000 000 000)^op2_shift
 */
static DAI_ret_t DAI_add_shift(DAI_t rop, DAI_t op1, DAI_prec_t op1_shift, DAI_t op2, DAI_prec_t op2_shift)
{
  assert(rop != op1 && rop != op2);

  if (DAI_IS_ZERO(*op1) && DAI_IS_ZERO(*op2))
  {
    rop->flags |= DAI_FLAGS_ZERO;
    return DAI_RET_OK;
  }
  else
    rop->flags &= ~DAI_FLAGS_ZERO;

  DAI_prec_t min_prec = op1->prec > op2->prec ? op1->prec : op2->prec;
  DAI_prec_t max_shift = op1_shift > op2_shift ? op1_shift : op2_shift;
  DAI_t lower_op = op1_shift < op2_shift ? op1 : op2;

  // printf("%" PRIu64 " < %" PRIu64 "\n", rop->prec, min_prec);

  if (rop->prec < min_prec)
    return DAI_RET_PREC_ERROR;

  DAI_add_carry_t *carrys = calloc(min_prec + 1, sizeof(DAI_add_carry_t));
  if (carrys == NULL)
    return DAI_RET_MEM_ERROR;

  DAI_prec_t min_shift = op1_shift < op2_shift ? op1_shift : op2_shift;

  for (DAI_prec_t i = min_shift; i < min_prec; ++i)
  {
    if (i < max_shift)
    {
      rop->data[i] = lower_op->data[i - min_shift];
      continue;
    }

    if (i - op1_shift >= op1->prec || i - op2_shift >= op2->prec)
      continue;

    rop->data[i] = op1->data[i - op1_shift] + op2->data[i - op2_shift];
    if (rop->data[i] >= DAI_DEC_UNIT_MAX)
    {
      rop->data[i] -= DAI_DEC_UNIT_MAX;
      carrys[i + 1] = 1;
    }
  }

  if (carrys[min_prec] != 0)
  {
    if (rop->prec < min_prec + 1)
      return DAI_RET_PREC_ERROR;
  }

  DAI_CHECK_RET_VALUE(DAI_handle_carrys(rop, carrys, min_prec + 1));

  free(carrys);

  return DAI_RET_OK;
}

/*
 * computes rop = op1 - op2
 */
DAI_ret_t DAI_sub(DAI_t rop, DAI_t op1, DAI_t op2)
{
  assert(rop != op1 && rop != op2);

  if (DAI_IS_ZERO(*op1) && DAI_IS_ZERO(*op2))
  {
    rop->flags |= DAI_FLAGS_ZERO;
    return DAI_RET_OK;
  }
  else
    rop->flags &= ~DAI_FLAGS_ZERO;

  DAI_prec_t max_prec = op1->prec > op2->prec ? op1->prec : op2->prec;

  if (rop->prec < max_prec)
    return DAI_RET_PREC_ERROR;

  // uint8_t carrys_was_set = 0;
  DAI_add_carry_t *carrys = calloc(max_prec + 1, sizeof(DAI_add_carry_t));
  if (carrys == NULL)
    return DAI_RET_MEM_ERROR;

  for (DAI_prec_t i = 0; i < max_prec; ++i)
  {
    if (op2->data[i] > op1->data[i]) // division would go negative
    {
      rop->data[i] = (DAI_DEC_UNIT_MAX + op1->data[i]) - op2->data[i];
      carrys[i + 1] = -1;
      // carrys_was_set = 1;
      continue;
    }

    rop->data[i] = op1->data[i] - op2->data[i];
  }

  if (carrys[max_prec] < 0)
  {
    if (rop->prec < max_prec + 1)
      return DAI_RET_PREC_ERROR;
  }

  DAI_CHECK_RET_VALUE(DAI_handle_carrys(rop, carrys, max_prec + 1));

  DAI_CHECK_RET_VALUE(DAI_correct_flags(rop), "Unreachable !!! :");

  free(carrys);

  return DAI_RET_OK;
}

/*
 * computes rop = op1 * op2
 * rop->prec MUST be at least 2, as the result of op1 * op2 could be up to 1 + 1 dec unit long
 */
static DAI_ret_t DAI_mul_dec_unit(DAI_t rop, DAI_dec_unit_t op1, DAI_dec_unit_t op2)
{
  if (rop->prec < 2)
  {
    fprintf(stderr, DAI_ERROR "the precision of the given ret value was not enough: %" PRIPREC " < %" PRIu64 "."
                              " If you're not an experienced developper you should not use DAI_mul_dec_unit by yourself."
                              " If you did not call this function yourself, please report this bug to the github repo. (%s)\n",
            rop->prec, 2ULL, "https://www.github.com/ThePythonCoder2006/Pi-chuck");
    return DAI_RET_PREC_ERROR;
  }

  DAI_set_zero(rop);

  uint64_t res = (uint64_t)op1 * (uint64_t)op2;
  if (res == 0)
  {
    DAI_SET_FLAG(*rop, DAI_FLAGS_ZERO);
    return DAI_RET_OK;
  }
  else
    DAI_CLEAR_FLAG(*rop, DAI_FLAGS_ZERO);

  // printf("res = %llu\n", res);
  rop->data[0] = res % DAI_DEC_UNIT_MAX;
  rop->data[1] = res / DAI_DEC_UNIT_MAX;

  return DAI_RET_OK;
}

DAI_ret_t DAI_mul_smol_int(DAI_t rop, DAI_t op1, uint32_t op2)
{
  DAI_set_zero(rop);
  DAI_t acc = rop;
  DAI_t acc2;
  DAI_init(&acc2, acc->prec);

  DAI_t digit_res;
  DAI_init(&digit_res, 2);

  for (DAI_prec_t i = 0; i < op1->prec; ++i)
  {
    DAI_mul_dec_unit(digit_res, op1->data[i], op2);
    DAI_add(acc2, acc, digit_res);

    // pointer swap
    DAI_t tmp = acc;
    acc = acc2;
    acc2 = tmp;
  }

  return DAI_RET_OK;
}

DAI_ret_t DAI_mul(DAI_t rop, DAI_t op1, DAI_t op2)
{
  assert(rop != op1 && rop != op2);

  if (DAI_IS_ZERO(*op1) || DAI_IS_ZERO(*op2))
  {
    DAI_SET_FLAG(*rop, DAI_FLAGS_ZERO);
    return DAI_RET_OK;
  }

  DAI_prec_t min_prec = op1->prec > op2->prec ? op1->prec : op2->prec;

  if (rop->prec < min_prec)
    return DAI_RET_PREC_ERROR;

  if (op1->prec > DAI_PREC_MAX - op2->prec)
  {
    fprintf(stderr, DAI_ERROR "the precision of the ints provided adds up to more than %" PRIPREC " (DAI_PREC_MAX), which is the maximum supported\n",
            DAI_PREC_MAX);
    return DAI_RET_PREC_ERROR;
  }
  // DAI_prec_t max_prec = op1->prec + op2->prec;

  DAI_set_zero(rop);
  DAI_t acc = rop; // acc points to the same place as rop
  DAI_t acc2;
  DAI_init(&acc2, acc->prec);

  DAI_t digit_res;
  DAI_init(&digit_res, 2);
  for (DAI_prec_t i = 0; i < op2->prec; ++i)
  {
    for (DAI_prec_t j = 0; j < op1->prec; ++j)
    {
      DAI_CHECK_RET_VALUE(DAI_mul_dec_unit(digit_res, op2->data[i], op1->data[j]));

      DAI_CHECK_RET_VALUE(DAI_add_shift(acc2, acc, 0, digit_res, i + j));
      // pointer swap
      DAI_t tmp = acc;
      acc = acc2;
      acc2 = tmp;
    }
  }

  return DAI_RET_OK;
}

/*
 * sets rop < 0 <=> A  < B
 *      rop = 0 <=> A == B
 *      rop > 0 <=> A  > B
 */
DAI_ret_t DAI_cmp(int8_t *rop, DAI_t A, DAI_t B)
{
  if (DAI_IS_ZERO(*A))
  {
    if (DAI_IS_ZERO(*B))
      *rop = 0;
    else
    {
      if (DAI_IS_NEGA(*B))
        *rop = 1;
      else
        *rop = -1;
    }
    return DAI_RET_OK;
  }

  if (DAI_IS_ZERO(*B))
  {
    if (DAI_IS_NEGA(*A))
      *rop = -1;
    else
      *rop = 1;
    return DAI_RET_OK;
  }

  if (DAI_IS_NEGA(*A))
  {
    if (!DAI_IS_NEGA(*B))
    {
      *rop = -1;
      return DAI_RET_OK;
    }
  }

  DAI_t max_prec_op = A->prec >= B->prec ? A : B;
  DAI_t min_prec_op = A->prec >= B->prec ? B : A;

  DAI_prec_t max_prec = A->prec >= B->prec ? A->prec : B->prec;
  DAI_prec_t min_prec = A->prec >= B->prec ? B->prec : A->prec;

  for (DAI_prec_t i = max_prec - 1; i > min_prec; --i)
    if (max_prec_op->data[i] != 0)
    {
      // max_prec_op > min_prec_op
      *rop = 2 * (max_prec_op == A) - 1;
      return DAI_RET_OK;
    }

  if (max_prec > min_prec)
    /*
     * check case i = min_prec
     * this needs to be separated in order to work in the case min_prec = 0
     */
    if (max_prec_op->data[min_prec] != 0)
    {
      // max_prec_op > min_prec_op
      *rop = 2 * (max_prec_op == A) - 1;
      return DAI_RET_OK;
    }

  for (DAI_prec_t i = min_prec - 1; i > 0; --i)
  {
    if (max_prec_op->data[i] == min_prec_op->data[i])
      continue;

    if (max_prec_op->data[i] > min_prec_op->data[i])
      *rop = 2 * (max_prec_op == A) - 1;
    else // min_prec_op < max_prec_op
      *rop = 2 * (min_prec_op == A) - 1;
    return DAI_RET_OK;
  }

  // default value
  *rop = 0;

  // testing cas i = 0
  if (max_prec_op->data[0] > min_prec_op->data[0])
    *rop = 2 * (max_prec_op == A) - 1;
  else if (max_prec_op->data[0] < min_prec_op->data[0])
    *rop = 2 * (min_prec_op == A) - 1;

  return DAI_RET_OK;
}

DAI_ret_t DAI_print(DAI_t op)
{
  if ((op->flags | DAI_FLAGS_ZERO) == op->flags)
  {
    printf("0");
    return DAI_RET_OK;
  }

  DAI_prec_t start = 0;
  while (op->data[op->prec - (start++) - 1] == 0 && start < op->prec)
    ;
  printf("%u ", op->data[op->prec - start]);
  for (DAI_prec_t i = start; i < op->prec; ++i)
    printf("%09u ", op->data[op->prec - i - 1]);

  return DAI_RET_OK;
}

DAI_errormsg *DAI_strerror(DAI_ret_t op)
{
  if (op >= DAI_RET_TOT)
    return &DAI_errormsgs[DAI_RET_TOT];
  return &DAI_errormsgs[op];
}

#endif // __DAI_USE_GMP__