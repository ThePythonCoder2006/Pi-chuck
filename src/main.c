#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#include "DAI.h"

#define PREC (200)

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
  DAI_INIT(&S, PREC);

  DAI_CHECK_RET_VALUE(DAI_set_ui(A, (1234567890)));
  DAI_CHECK_RET_VALUE(DAI_set_ui(B, (BILLION)));

  uint64_t n = 2;

  printf("  ");
  DAI_print(A);
  printf("\n>>%-10llu", n);
  // DAI_print(B);
  printf("\n______________________________\n");

  DAI_CHECK_RET_VALUE(DAI_srl(S, A, n), "could not shift A by 5 : ");

  printf("\n  ");
  DAI_print(S);
  // printf("A %c B\n", rop < 0 ? '<' : (rop == 0 ? '=' : '>'));
  putchar('\n');

  DAI_clean(&A);
  DAI_clean(&B);
  DAI_clean(&S);

  return 0;
}

/*
 * computes (426880 * sqrt(10005)) * 2 * half using coupled Newton's method:
 * r_approx = 0.01 * half ~= 1/sqrt(10005) * half
 * s_0 = 10005 * r_approx
 * r_0 = r_approx / 2
 *
 * s_{n+1} = s_n + r_n * (10005 - s_n^2)
 * r_{n+1} = r_n + r_n * (1 - 2 * r_n * s_{n+1})
 *
 * where (s_n) becomes a better approximation to sqrt(10005)
 */
int compute_const(DAI_t rop, DAI_t half)
{
  DAI_t r_approx;

  return 0;
}
