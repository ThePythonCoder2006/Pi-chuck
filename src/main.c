#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#include "DAI.h"

#define PREC (5)

int main(int argc, char **argv)
{
  (void)argc, (void)argv;

  // uint64_t digit_count = 0;
  // if (argc < 2)
  // {
  //   // ask the user for the number of digits to be computed
  //   printf("How many digits of pi do you wish to compute ? ");
  //   scanf("%" SCNu64, &digit_count);
  // }
  // else
  //   digit_count = _atoi64(argv[1]);

  // printf("computing pi with a precision of %" PRIu64 " digits\n", digit_count);

  DAI_t A, B, S;
  DAI_init(&A, PREC);
  DAI_init(&B, PREC);
  DAI_init(&S, PREC);

  DAI_CHECK_RET_VALUE(DAI_set_ui(A, DAI_DEC_UNIT_MAX * DAI_DEC_UNIT_MAX - 1))
  DAI_CHECK_RET_VALUE(DAI_set_ui(B, 10 * DAI_DEC_UNIT_MAX - 1));

  printf("  ");
  DAI_print(A);
  printf("\nx ");
  DAI_print(B);
  printf("\n______________________________\n");

  DAI_CHECK_RET_VALUE(DAI_mult(S, A, B), "could not multiply A and B : ");

  printf("\n= ");
  DAI_print(S);
  putchar('\n');

  DAI_clean(&A);
  DAI_clean(&B);
  DAI_clean(&S);

  return 0;
}

/*
 * compute rop = sqrt(op * one)
 */
DAI_ret_t sqrt_coupled_newtons_iteration(DAI_t rop, DAI_t op, DAI_t half)
{
  if (op->prec > DAI_PREC_MAX - half->prec) // op->prec + one->prec > DAI_PREC_MAX
    return DAI_RET_PREC_ERROR;

  if (rop->prec < (op->prec + half->prec) / 2)
    return DAI_RET_PREC_ERROR;

  DAI_t tmp;
  DAI_init(&tmp, rop->prec);

  DAI_t op_half;
  DAI_init(&op_half, op->prec + half->prec);
  DAI_mult(op_half, op, half);

  DAI_t x_n = rop;
  DAI_set(x_n, op_half); // initial guess
  DAI_t y_n;
  DAI_init(&y_n, rop->prec);
  DAI_set_ui(y_n, 1);

  DAI_t op_one;
  DAI_init(&op_one, op_half->prec);
  DAI_mult_smol_int(op_one, op_half, 1);

  DAI_t two;
  DAI_init(&two, half->prec + 1);
  DAI_mult_smol_int(two, half, 4);

  DAI_dec_unit_t x_n_old = x_n->data[0];

  while (1)
  {
    x_n_old = x_n->data[0];

    // y_n = y_n * ( 2 - 2* x_n * y_n)
    DAI_mult(tmp, x_n, y_n); // tmp = x_n * x_n
    DAI_mult(y_n, tmp, two); // y_n = 2 * x_n * y_n
    DAI_sub(tmp, two, y_n);  // tmp = 2 - 2 * x_n * y_n

    if (x_n_old == x_n->data[0])
      break;
  }

  DAI_clean(op_one);

  return DAI_RET_OK;
}
