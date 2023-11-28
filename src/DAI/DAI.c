#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <assert.h>

#include "DAI.h"

DAI_errormsg DAI_errormsgs[DAI_RET_TOT + 1] = {
    "nothing went wrong !",
    "the precision of the int given was not enough to perform the calculations that were asked !",
    "some memory could not be allocated !"
    "the error code given does not exist !",
};

DAI_ret_t DAI_CHECK_ret = DAI_RET_OK;

DAI_ret_t
DAI_init(DAI_ptr rop, DAI_prec_t prec)
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

DAI_ret_t DAI_set_zero(DAI_t rop)
{
  rop->flags = DAI_FLAGS_ZERO;
  for (DAI_prec_t i = 0; i < rop->prec; ++i)
    rop->data[i] = 0;

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

DAI_ret_t DAI_handle_carrys(DAI_t rop, uint8_t *carrys, size_t carrys_size)
{
  assert(carrys_size > 0);

  uint8_t *carrys2 = calloc(carrys_size, sizeof(uint8_t));

  uint8_t quit = 0;
  while (!quit)
  {
    quit = 1;
    for (DAI_prec_t i = 1; i < carrys_size - 1; ++i)
    {
      rop->data[i] += carrys[i];
      if (rop->data[i] >= DAI_DEC_UNIT_MAX)
      {
        rop->data[i] -= DAI_DEC_UNIT_MAX;
        carrys2[i + 1] += 1;
        quit = 0;
      }
    }

    memcpy(carrys, carrys2, carrys_size * sizeof(uint8_t));
  }

  return DAI_RET_OK;
}

DAI_ret_t DAI_add(DAI_t rop, DAI_t op1, DAI_t op2)
{
  assert(rop != op1 && rop != op2);

  if (DAI_IS_ZERO(op1) && DAI_IS_ZERO(op2))
  {
    rop->flags |= DAI_FLAGS_ZERO;
    return DAI_RET_OK;
  }
  else
    rop->flags &= ~DAI_FLAGS_ZERO;

  DAI_prec_t max_prec = op1->prec > op2->prec ? op1->prec : op2->prec;

  if (rop->prec < max_prec)
    return DAI_RET_PREC_ERROR;

  uint8_t *carrys = calloc(max_prec + 1, sizeof(uint8_t));

  for (DAI_prec_t i = 0; i < max_prec; ++i)
  {
    rop->data[i] = op1->data[i] + op2->data[i];
    if (rop->data[i] >= DAI_DEC_UNIT_MAX)
    {
      rop->data[i] -= DAI_DEC_UNIT_MAX;
      carrys[i + 1] = 1;
    }
  }

  if (carrys[max_prec] != 0)
  {
    if (rop->prec < max_prec + 1)
      return DAI_RET_PREC_ERROR;
  }

  uint8_t *carrys2 = calloc(max_prec + 1, sizeof(uint8_t));

  uint8_t quit = 0;
  while (!quit)
  {
    quit = 1;
    for (DAI_prec_t i = 1; i < max_prec; ++i)
    {
      rop->data[i] += carrys[i];
      if (rop->data[i] >= DAI_DEC_UNIT_MAX)
      {
        rop->data[i] -= DAI_DEC_UNIT_MAX;
        carrys2[i + 1] += 1;
        quit = 0;
      }
    }

    memcpy(carrys, carrys2, (max_prec + 1) * sizeof(uint8_t));
  }

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
  while (op->data[op->prec - (start++) - 1] == 0)
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

