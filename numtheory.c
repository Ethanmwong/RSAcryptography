#include "numtheory.h"
#include "randstate.h"

void gcd(mpz_t g, const mpz_t a, const mpz_t b) {
    mpz_t aa, bb, temp;
    mpz_init(temp);
    mpz_init_set(aa, a);
    mpz_init_set(bb, b);
    while (mpz_cmp_ui(bb, 0) != 0) { // while (bb != 0)
        mpz_set(temp, bb); // temp = bb
        mpz_mod(bb, aa, bb); // bb = aa mod bb
        mpz_set(aa, temp); // aa = temp
    }
    mpz_set(g, aa); // g = aa
    mpz_clears(aa, bb, temp, NULL); // clear aa, bb, temp
}

void mod_inverse(mpz_t o, const mpz_t a, const mpz_t n) {
    mpz_t r, r_prime, t, t_prime, q, temp, temp_mul, temp1, temp_mul1;
    mpz_inits(q, temp, temp_mul, temp1, temp_mul1, NULL);
    mpz_init_set(r, n); // r = n
    mpz_init_set(r_prime, a); // r_prime = a
    mpz_init_set_ui(t, 0); // t = 0
    mpz_init_set_ui(t_prime, 1); // t_prime = 1
    while (mpz_cmp_ui(r_prime, 0) != 0) { // while r_prime != 0
        mpz_fdiv_q(q, r, r_prime); // q = r / r_prime

        mpz_set(temp, r); //set temp to r
        mpz_set(r, r_prime); // r = r'
        mpz_mul(temp_mul, q, r_prime); // temp_mul = q * r_prime
        mpz_sub(r_prime, temp, temp_mul); // r_prime = r - q * r_prime

        mpz_set(temp, t); //set temp to t
        mpz_set(t, t_prime); // t = t'
        mpz_mul(temp_mul, q, t_prime); // temp_mul = q * t_prime
        mpz_sub(t_prime, temp, temp_mul); // t_prime = t - q * t_prime
    }
    if (mpz_cmp_ui(r, 1) > 0) { // if r > 1
        mpz_set_ui(o, 0); // o = 0
        mpz_clears(r, r_prime, t, t_prime, q, temp, temp_mul, temp1, temp_mul1, NULL);
        return;
    }
    if (mpz_cmp_ui(t, 0) < 0) { // if t < 0
        mpz_add(t, t, n); // t = t + n
    }
    mpz_set(o, t); // o = t
    mpz_clears(r, t, r_prime, t_prime, q, temp, temp_mul, temp1, temp_mul1, NULL);
    return;
}

void pow_mod(mpz_t o, const mpz_t a, const mpz_t d, const mpz_t n) {
    mpz_t v, p, dd, aa;
    mpz_inits(v, p, dd, aa, NULL);
    mpz_set_ui(v, 1); // v = 1
    mpz_set(p, a); // p = a
    mpz_set(dd, d); // dd = d
    while (mpz_cmp_ui(dd, 0) > 0) { // while dd > 0
        if (mpz_odd_p(dd) != 0) { // if dd is odd
            mpz_mul(v, v, p); // v = v * p
            mpz_mod(v, v, n); // v = v mod n
        }
        mpz_mul(p, p, p); // p = p * p
        mpz_mod(p, p, n); // p = p mod n
        mpz_fdiv_q_ui(dd, dd, 2); // dd = dd / 2
    }
    mpz_set(o, v); // o = v
    mpz_clears(v, p, dd, aa, NULL); // clear v, p, dd, aa
    return;
}

//Lines 73 - 78 taken from primes.py in the python resources folder.
bool is_prime(const mpz_t n, uint64_t iters) {
    if (mpz_cmp_ui(n, 2) < 0 || (mpz_cmp_ui(n, 2) != 0 && mpz_even_p(n) != 0)) {
        return false;
    }
    if (mpz_cmp_ui(n, 2) == 0 || mpz_cmp_ui(n, 3) == 0) {
        return true;
    }
    mpz_t a, nn, num_2, n_minus_1, n_minus_2, y, s, s_minus_one, r, j;
    mpz_inits(a, nn, n_minus_1, n_minus_2, y, s, s_minus_one, r, j, NULL);
    mpz_sub_ui(n_minus_1, n, 1); // n_minus_1 = n - 1
    mpz_set(r, n_minus_1); // r = n - 1
    mpz_set_ui(s, 0); // s = 0
    while (mpz_even_p(r) != 0) { // while r is even
        mpz_fdiv_q_ui(r, r, 2); // r = r / 2
        mpz_add_ui(s, s, 1); // s = s + 1
    }
    mpz_init_set_ui(num_2, 2); // num_2 = 2
    mpz_sub_ui(n_minus_2, n, 2); // n_minus_2 = n - 2
    mpz_sub_ui(s_minus_one, s, 1); // s_minus_one = s - 1
    for (uint64_t i = 1; i < iters; i++) { // for i = 1 to iters
        mpz_urandomm(a, state, n_minus_2); // a = random number between 0 and n-2
        mpz_add_ui(a, a, 2); // a = a + 2, so now a is between 2 and n-2
        pow_mod(y, a, r, n); // y = a^r mod n
        if (mpz_cmp_ui(y, 1) != 0 && mpz_cmp(y, n_minus_1) != 0) { // if y != 1 and y != n-1
            mpz_set_ui(j, 1); // j = 1
            while (mpz_cmp(j, s_minus_one) <= 0
                   && mpz_cmp(y, n_minus_1) != 0) { // while j < s-1 and y != n-1
                pow_mod(y, y, num_2, n); // y = y^2 mod n
                if (mpz_cmp_ui(y, 1) == 0) { // if y == 1
                    mpz_clears(a, nn, n_minus_1, n_minus_2, y, s, s_minus_one, r, j, num_2, NULL);
                    return false;
                }
                mpz_add_ui(j, j, 1); // j = j + 1
            }
            if (mpz_cmp(y, n_minus_1) != 0) { // if y != n-1
                mpz_clears(a, nn, n_minus_1, n_minus_2, y, s, s_minus_one, r, j, num_2, NULL);
                return false;
            }
        }
    }
    mpz_clears(a, nn, n_minus_1, n_minus_2, y, s, s_minus_one, r, j, num_2, NULL);
    return true;
}

void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {
    mpz_t one, range;
    mpz_inits(one, range, NULL);
    mpz_set_ui(one, 1); // one = 1
    mpz_mul_2exp(range, one,
        bits); // range = 2^bits, so the range for the random number is 2^bits to 2^(bits+1)
    do { // while p is not prime
        do { // while p is even
            mpz_urandomb(p, state, bits); //generate a random number
        } while (mpz_even_p(p) != 0);
        mpz_add(p, p, range); // p = p + 2^(bits). This makes it so p is at least bits bits long
    } while (!is_prime(p, iters));
    mpz_clears(one, range, NULL);
}

// int main(void) {
//     //test make prime and is prime
//     mpz_t p,q,n,phi_n,e,d;
//     mpz_inits(p,q,n,phi_n,e,d, NULL);
//     randstate_init(1);
//     //for all numbers between 1 and 1000, check if they are prime
//     for (uint64_t i = 4; i < 1000; i++) {
//         mpz_set_ui(p,i);
//         if (is_prime(p, 100)) {
//             gmp_printf("%Zd is prime\n", p);
//         }
//     }
//     return 0;
// }
