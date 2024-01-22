#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>

#include "DAI.h"

#define PREC (200)

int compute_const(DAI_t rop, uint64_t n);

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
  DAI_10_pow_n(A, 18);
  DAI_print(A);
  putchar('\n');

  // compute_const(S, 200);
  DAI_mul_smol_int(S, A, 384);
  DAI_print(S);
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
 * r_{n + 1} = (4 * (r_n * one) * ( half - ((r_n * one) * (s_{n + 1} * one))/one)) / one = one * (2 * r_n * (1 - r_n * s_{n + 1}))
 *
 * where (s_n) becomes a better approximation to sqrt(10005)
 *
 * /!\ n must be greater or equal to 1
 */
int compute_const(DAI_t rop, uint64_t n)
{
  assert(n >= 1);

  const uint32_t root_int = 10005;
  const uint32_t half_int = 5;

  DAI_t half;
  DAI_INIT(&half, n);
  DAI_10_pow_n(half, n - 1);                                 // use rop as a temporary storage
  half->data[(n - 1) / DAI_DEC_UNIT_NUM_DIGITS] *= half_int; // 0000000001000000 * 5 = 0000000005000000
  DAI_print(half);
  putchar('\n');

  DAI_t root;
  DAI_INIT(&root, n);
  DAI_mul_smol_int(root, half, root_int * 2); // root = 2 * half * 10005 = 10005 * one
  DAI_print(root);
  putchar('\n');

  DAI_t r_approx;
  DAI_INIT(&r_approx, rop->prec);
  DAI_srl_dec(r_approx, half, 2); // r_approx = half * 1/100 = 0.01 * half

  DAI_t s_n, r_n;
  DAI_INIT(&s_n, rop->prec);
  r_n = r_approx;

  DAI_t s_0 = s_n;
  DAI_t r_0 = r_n;
  DAI_mul_smol_int(s_0, r_approx, 2 * root_int); // s_0 = r_approx * (10005 * 2) = 0.01 * 10005 * half * 2 = 0.01 * 10005 * one
  // r_0 = r_approx ~= 1/2sqrt(10005) * one

  DAI_t acc;
  DAI_INIT(&acc, rop->prec);

  // loop
  // for (uint64_t _ = 0; _ < log2(n); ++_)
  {
    DAI_set(acc, s_n);
    // s_{n + 1} = one * (s_n + r_n * (10005 - s_n^2))
    DAI_mul(rop, s_n, s_n);  // rop = s_n^2 * one^2
    DAI_srl(s_n, rop, n);    // s_n = s_n^2 * one
    DAI_sub(rop, root, s_n); // rop = (10005 * one) - (s_n^2 * one)
    DAI_mul(s_n, r_n, rop);  // s_n = (r_n * one) * ((10005 - s_n^2) * one)
    DAI_srl(rop, s_n, n);    // rop = r_n * (10005 - s_n^2) * one
    DAI_add(s_n, acc, rop);  // s_n = s_n + r_n * (10005 - s_n^2) = s_{n + 1}

    DAI_set(acc, r_n);
    // r_{n + 1} = one * ( 4 * r_n * ( 0.5 - r_n * s_{n + 1}))
    DAI_mul(rop, r_n, s_n);  // rop = (r_n * one) * (s_{n + 1} * one)
    DAI_srl(r_n, rop, n);    // r_n = (r_n * s_{n + 1}) * one
    DAI_sub(rop, half, r_n); // rop = (0.5 * one) - ((r_n * s_{n + 1}) * one) = one * (.5 - r_n * s_{n+1})
    DAI_mul(r_n, acc, rop);  // r_n = (r_n * one) * (one * (.5 - r_n * s_{n+1}))
    DAI_srl(rop, r_n, n);    // rop = one * (r_n * (.5 - r_n * s_{n+1}))
    DAI_mul_smol_int(r_n, rop, 4);
  }

  DAI_set(rop, s_n);

  // DAI_mul_smol_int(rop, s_n, 426880);

  return 0;
}
