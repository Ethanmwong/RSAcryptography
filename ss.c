#include "ss.h" // for rsa_init, rsa_clear, rsa_encrypt, rsa_decrypt

#include "numtheory.h" // for gcd, mod_inverse, pow_mod
#include "randstate.h" // for randstate_init, randstate_clear

#include <stdlib.h> // for malloc, free

void ss_make_pub(mpz_t p, mpz_t q, mpz_t n, uint64_t nbits, uint64_t iters) {
    mpz_t p_minus_one, q_minus_one;
    mpz_inits(p_minus_one, q_minus_one, NULL);
    uint64_t pbits
        = (nbits / 5)
          + (random() % (nbits / 5)); // random number of bits for p in range [nbits/5, 2nbits/5]
    uint64_t qbits = nbits - (pbits + pbits); // number of bits for q, which is the remainder
    do { // while p and q - 1 are not divisible by each other vice versa
        make_prime(p, pbits, iters); // generate a random prime p
        make_prime(q, qbits, iters); // generate a random prime q
        mpz_mul(n, p, p); // n = p * p
        mpz_mul(n, n, q); // n = p * p * q
        mpz_sub_ui(p_minus_one, p, 1); // p_minus_one = p - 1
        mpz_sub_ui(q_minus_one, q, 1); // q_minus_one = q - 1
    } while ((mpz_divisible_p(p_minus_one, q) != 0) || (mpz_divisible_p(q_minus_one, p) != 0));
    mpz_clears(p_minus_one, q_minus_one, NULL);
}

void ss_make_priv(mpz_t d, mpz_t pq, const mpz_t p, const mpz_t q) {
    mpz_t p_minus_one, q_minus_one, totient, numerator, denominator, n;
    mpz_inits(p_minus_one, q_minus_one, totient, numerator, denominator, n, NULL);
    mpz_sub_ui(p_minus_one, p, 1); // p_minus_one = p - 1
    mpz_sub_ui(q_minus_one, q, 1); // q_minus_one = q - 1
    mpz_mul(numerator, p_minus_one, q_minus_one); // numerator = p_minus_one * q_minus_one
    mpz_abs(numerator, numerator); // numerator = |numerator|
    gcd(denominator, p_minus_one, q_minus_one); // gcd = gcd(p_minus_one, q_minus_one)
    mpz_div(totient, numerator, denominator); // totient = numerator / denominator
    mpz_mul(pq, p, q); // pq = p * q
    mpz_mul(n, p, p); // n = p * q
    mpz_mul(n, n, q); // n = n * p
    mod_inverse(d, n, totient); // d = pq^-1 mod totient
    mpz_clears(p_minus_one, q_minus_one, totient, numerator, denominator, n, NULL); // free memory
}

void ss_write_pub(const mpz_t n, const char username[], FILE *pbfile) {
    gmp_fprintf(pbfile, "%Zx\n%s\n", n, username); // write n and username to pbfile
}

void ss_write_priv(const mpz_t pq, const mpz_t d, FILE *pvfile) {
    gmp_fprintf(pvfile, "%Zx\n%Zx\n", pq, d); // write pq and d to pvfile
}

void ss_read_pub(mpz_t n, char username[], FILE *pbfile) {
    gmp_fscanf(pbfile, "%Zx\n%s\n", n, username); // read n and username from pbfile
}

void ss_read_priv(mpz_t pq, mpz_t d, FILE *pvfile) {
    gmp_fscanf(pvfile, "%Zx\n%Zx\n", pq, d); // read pq and d from pvfile
}

void ss_encrypt(mpz_t c, const mpz_t m, const mpz_t n) {
    pow_mod(c, m, n, n); // c = m^n mod n
}

void ss_encrypt_file(FILE *infile, FILE *outfile, const mpz_t n) {
    mpz_t m, c, nn;
    mpz_inits(m, c, nn, NULL);
    uint64_t k = (0.5 * mpz_sizeinbase(n, 2) - 1)
                 / 8; // log_2(sqrt(n) - 1) / 8 is equivalent to (1/2 * log_2(n) - 1 )/ 8
    uint8_t *block = (uint8_t *) malloc(k); // allocate memory for block
    block[0] = 0xFF; // set zeroth byte of block to 0xFF
    size_t j;
    while ((j = fread(block + 1, sizeof(uint8_t), k - 1, infile))
           > 0) { // while there are still unprocessed bytes in infile
        mpz_import(m, j + 1, 1, sizeof(uint8_t), 1, 0, block); // convert read bytes into an mpz_t.
        ss_encrypt(c, m, n); // c = m^n mod n
        gmp_fprintf(outfile, "%Zx\n", c); // write c to outfile as a hexstring
    }
    free(block); // free memory
    mpz_clears(m, c, nn, NULL); // free memory
}

void ss_decrypt(mpz_t m, const mpz_t c, const mpz_t d, const mpz_t pq) {
    pow_mod(m, c, d, pq); // m = c^d mod pq
}

void ss_decrypt_file(FILE *infile, FILE *outfile, const mpz_t d, const mpz_t pq) {
    mpz_t c, m;
    mpz_inits(c, m, NULL);
    uint64_t k = (mpz_sizeinbase(pq, 2) - 1) / 8; // k = floor((log_2(pq) - 1) / 8)
    uint8_t *block = (uint8_t *) malloc(k); // allocate memory for block that can hold k bytes
    size_t j;
    while ((gmp_fscanf(infile, "%Zx\n", c)) == 1) { // while not at end of file
        ss_decrypt(m, c, d, pq); // m = c^d mod pq
        mpz_export(block, &j, 1, sizeof(uint8_t), 1, 0, m); // export m to block
        fwrite(block + 1, sizeof(uint8_t), j - 1,
            outfile); // write j - 1 bytes from index 1 of block to outfile
    }
    free(block);
    mpz_clears(c, m, NULL);
}
