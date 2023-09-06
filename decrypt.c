//clang-format off
#include <stdio.h> // for printf
#include <gmp.h> // for gmp
//clang-format on
#include <stdlib.h> // for exit
#include <stdint.h> // for uint64_t
#include <time.h> // for time
#include <getopt.h> // for getopt
#include <sys/types.h> // for open
#include "randstate.h"
#include "ss.h"

#define OPTIONS "ionvh :"

static void usage(void) {
    fprintf(stderr, "SYNOPSIS\n"
                    "  Decrypts data using SS encryption.\n"
                    "  Encrypted data is encrypted by the encrypt program.\n"
                    "\n"
                    "USAGE\n"
                    "   ./decrypt [OPTIONS]"
                    "\n"
                    "OPTIONS\n"
                    "   -h              Display program help and usage.\n"
                    "   -v              Display verbose program output.\n"
                    "   -i infile       Input file of data to decrypt (default: stdin).\n"
                    "   -o outfile      Output file for decrypted data (default: stdout).\n"
                    "   -n pvfile       Private key file (default: ss.priv).\n");
}

int main(int argc, char *argv[]) {
    char *private_key_file = "ss.priv"; // private key file
    FILE *inp = stdin; // input file
    FILE *out = stdout; // output file
    bool verbose = 0; // verbose output
    mpz_t pq, d;
    mpz_inits(pq, d, NULL);
    int opt;
    while ((opt = getopt(argc, argv, "i:o:n:vh")) != -1) {
        switch (opt) {
        case 'i':
            inp = fopen(optarg, "r");
            if (inp == NULL) {
                fprintf(stderr, "ss.priv: No such file or directory.\n");
                return -1;
            }
            break;
        case 'o':
            out = fopen(optarg, "w");
            if (out == NULL) {
                fprintf(stderr, "ss.priv: No such file or directory.\n");
                return -1;
            }
            break;
        case 'n': private_key_file = optarg; break;
        case 'v': verbose = 1; break;
        case 'h': usage(); return 0;
        default: usage(); return -1;
        }
    }
    FILE *priv_key_file = fopen(private_key_file, "r");
    if (priv_key_file == NULL) {
        fprintf(stderr, "ss.priv: No such file or directory\n");
        return -1;
    }
    ss_read_priv(pq, d, priv_key_file); // read private key from file
    if (verbose == 1) {
        gmp_fprintf(stdout, "pq (%lu bits) = %Zd\n", mpz_sizeinbase(pq, 2), pq);
        gmp_fprintf(stdout, "d  (%lu bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
    }
    ss_decrypt_file(inp, out, d, pq); // decrypt file
    fclose(priv_key_file); // close private key file
    fclose(inp); // close input file
    fclose(out); // close output file
    mpz_clears(pq, d, NULL); // clear mpz_t variables
    return 0;
}
