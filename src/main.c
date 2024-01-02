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
  DAI_INIT(&A, PREC);
  DAI_INIT(&B, PREC);
  DAI_INIT(&S, 5 * PREC);

  DAI_CHECK_RET_VALUE(DAI_set_ui(A, 1ULL << 63));
  DAI_CHECK_RET_VALUE(DAI_set_ui(B, 1ULL << 63));

  printf("  ");
  DAI_print(A);
  printf("\nx ");
  DAI_print(B);
  printf("\n______________________________\n");

  DAI_CHECK_RET_VALUE(DAI_mul(S, A, B), "could not multiply A and B : ");

  printf("\n= ");
  DAI_print(S);
  putchar('\n');

  DAI_clean(&A);
  DAI_clean(&B);
  DAI_clean(&S);

  return 0;
}

/*
 * computes (426880 * sqrt(10005)) * one using the so called Newton's coupled iteration
 */
int compute_const(DAI_t rop, DAI_t one)
{

  return 0;
}
