//clang-format off
#include <stdio.h> // for printf
#include <gmp.h> // for gmp
//clang-format on
#include "numtheory.h" // for gcd, mod_inverse, pow_mod
#include "randstate.h" // for randstate_init, randstate_clear
#include "ss.h" // for rsa_keygen, rsa_encrypt, rsa_decrypt

#include <getopt.h> // for getopt_long
#include <stdbool.h> // for bool
#include <stdint.h> // for uint64_t
#include <stdlib.h> // for getenv
#include <sys/stat.h> // for open
#include <time.h> // for time

#define OPTIONS "bindsvh :"

static void usage(void) {
    fprintf(stderr, "SYNOPSIS\n"
                    "   Generates an SS public/private key pair."
                    "\n"
                    "USAGE\n"
                    "   ./keygen [OPTIONS]\n"
                    "\n"
                    "OPTIONS\n"
                    "   -h              Display program help and usage.\n"
                    "   -v              Display verbose program output.\n"
                    "   -b bits         Minimum bits needed for public key n (default: 256).\n"
                    "   -i iterations   Miller-Rabin iterations for testing primes (default: 50).\n"
                    "   -n pbfile       Public key file (default: ss.pub).\n"
                    "   -d pvfile       Private key file (default: ss.priv).\n"
                    "   -s seed         Random seed for testing.\n");
}

int main(int argc, char *argv[]) {
    uint64_t iters = 50; // default number of iterations
    char *pub_key_file = "ss.pub"; // default public key file
    char *priv_key_file = "ss.priv"; // default private key file
    uint64_t seed = time(NULL); // default seed
    uint64_t bits = 256; // default number of bits
    bool verbose = 0; // default verbose
    int opt;
    while ((opt = getopt(argc, argv, "b:i:n:d:s:vh")) != -1) {
        switch (opt) {
        case 'b': bits = strtoul(optarg, NULL, 10); break;
        case 'i': iters = strtoul(optarg, NULL, 10); break;
        case 'n': pub_key_file = optarg; break;
        case 'd': priv_key_file = optarg; break;
        case 's': seed = strtoul(optarg, NULL, 10); break;
        case 'v': verbose = 1; break;
        case 'h': usage(); return 0;
        default: usage(); return -1;
        }
    }
    FILE *pbfile = fopen(pub_key_file, "w"); // public key file
    FILE *pvfile = fopen(priv_key_file, "w"); // private key file
    fchmod(fileno(pvfile), 0600); // set private key file permissions to 0600 (user only)
    randstate_init(seed); // initialize random number generator
    mpz_t p, q, n, d, pq; // declare variables
    mpz_inits(p, q, n, d, pq, NULL); // initialize variables
    ss_make_pub(p, q, n, bits, iters); // generate public key
    ss_make_priv(d, pq, p, q); // generate private key
    char *username = getenv("USER"); // get username
    if (username == NULL) {
        fprintf(stderr, "Error: Cannot get username.\n");
        return -1;
    }
    ss_write_pub(n, username, pbfile); // write public key to file
    ss_write_priv(pq, d, pvfile); // write private key to file
    if (verbose == 1) { // if verbose is enabled
        fprintf(stdout, "user = %s\n", username);
        gmp_fprintf(stdout, "p  (%lu bits) = %Zd\n", mpz_sizeinbase(p, 2), p);
        gmp_fprintf(stdout, "q  (%lu bits) = %Zd\n", mpz_sizeinbase(q, 2), q);
        gmp_fprintf(stdout, "n  (%lu bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_fprintf(stdout, "pq (%lu bits) = %Zd\n", mpz_sizeinbase(pq, 2), pq);
        gmp_fprintf(stdout, "d  (%lu bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
    }
    mpz_clears(p, q, n, d, pq, NULL); // clear variables
    randstate_clear(); // clear random number generator
    fclose(pbfile); // close public key file
    fclose(pvfile); // close private key file
    return 0;
}
