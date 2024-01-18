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
 * computes (426880 * sqrt(10005)) * 10^n using coupled Newton's method:
 *
 * r_approx = 0.01 * half ~= 1/sqrt(10005) * half
 * s_0 = 10005 * r_approx * 2 = 10005 * 0.01 * one ~= sqrt(10005) * one
 * r_0 = r_approx = 1/2sqrt(10005) * one
 *
 * s_{n+1} * one = s_n * one + (r_n * one * (10005 * one - (s_n * one)^2 / one)) / one = one * (s_n + r_n * (10005 - s_n^2))
 * r_{n+1} = r_n * one + (r_n * one * (one - (2 * r_n * one * s_{n+1} * one) / one)) / one = one * (r_n + r_n * (1 - 2 * r_n * s_{n + 1}))
 *
 * where (s_n) becomes a better approximation to sqrt(10005)
 *
 * /!\ n must be greater or equal to 1
 */
int compute_const(DAI_t rop, uint64_t n)
{
  assert(n >= 1);

  const uint32_t root = 10005;
  const uint32_t half_int = 5;

  DAI_t one;
  DAI_INIT(one, rop->prec);
  DAI_10_pow_n(one, n);

  DAI_t r_approx;
  DAI_INIT(r_approx, rop->prec);
  DAI_srl_dec(r_approx, rop, 2); // r_approx = half * 1/100 = 0.01 * half

  DAI_t s_n, r_n;
  DAI_INIT(s_n, rop->prec);
  r_n = r_approx;

  DAI_t s_0 = s_n;
  DAI_t r_0 = r_n;
  DAI_mul_smol_int(s_0, r_approx, root); // s_0 = r_approx * 10005 * 2 = 0.01 * 10005 * half * 2 = 0.01 * 10005 * one
  // r_0 = r_approx ~= 1/2sqrt(10005) * one

  DAI_t acc;
  DAI_INIT(acc, rop->prec);

  // loop

  DAI_set(acc, s_n);
  // s_{n + 1} = s_n + r_n * (10005 - s_n^2)
  DAI_mul(rop, s_n, s_n);           // rop = s_n^2 * one^2
  DAI_smol_int_sub(s_n, root, rop); // s_n = 10005 - s_n^2 * one^2
  DAI_mul(rop, r_n, s_n);           // rop = r_n * (10005 - s_n^2)
  DAI_add(s_n, acc, rop);           // s_n = s_n + r_n * (10005 - s_n^2) = s_{n + 1}

  // r_{n + 1} = 2 * r_n * ( 1 - r_n * s_{n + 1})
  DAI_mul(rop, r_n, s_n); // rop = r_n * r_{n + 1}
  DAI_sub

      return 0;
}
