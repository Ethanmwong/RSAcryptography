//clang-format off
#include <stdio.h>
#include <gmp.h>
//clang-format on
#include "numtheory.h"
#include "randstate.h"
#include "ss.h"

#include <stdlib.h> // for exit
#include <sys/stat.h> // for open
#include <time.h> // for time
#include <getopt.h> // for getopt
#include <stdint.h> // for uint64_t

#define OPTIONS "ionvh :"

static void usage(void) {
    fprintf(stderr, "SYNOPSIS\n"
                    "   Encrypts data using SS encryption.\n"
                    "   Encrypted data is decrypted by the decrypt program.\n"
                    "\n"
                    "USAGE\n"
                    "   ./encrypt [OPTIONS]"
                    "\n"
                    "OPTIONS\n"
                    "   -h              Display program help and usage.\n"
                    "   -v              Display verbose program output.\n"
                    "   -i infile       Input file of data to encrypt (default: stdin).\n"
                    "   -o outfile      Output file for encrypted data (default: stdout).\n"
                    "   -n pbfile       Public key file (default: ss.pub).\n");
}
int main(int argc, char *argv[]) {
    int opt;
    char *public_key_file = "ss.pub"; // public key file
    FILE *inp = stdin; // input file
    FILE *out = stdout; // output file
    bool verbose = 0;
    mpz_t n;
    mpz_inits(n, NULL);
    while ((opt = getopt(argc, argv, "i:o:n:vh")) != -1) {
        switch (opt) {
        case 'i':
            inp = fopen(optarg, "r");
            if (inp == NULL) {
                fprintf(stderr, "%s: No such file or directory\n", optarg);
                return -1;
            }
            break;
        case 'o':
            out = fopen(optarg, "w");
            if (out == NULL) {
                fprintf(stderr, "ss.pub: No such file or directory\n");
                return -1;
            }
            break;
        case 'n': public_key_file = optarg; break;
        case 'v': verbose = 1; break;
        case 'h': usage(); return 0;
        default: usage(); return -1;
        }
    }
    FILE *pub_key_file = fopen(public_key_file, "r");
    if (pub_key_file == NULL) {
        fprintf(stderr, "ss.pub: No such file or directory\n");
        return -1;
    }
    char username;
    ss_read_pub(n, &username, pub_key_file); // read public key from file
    if (verbose == 1) {
        fprintf(stdout, "user = %s\n", &username);
        gmp_fprintf(stdout, "n (%lu bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
    }
    ss_encrypt_file(inp, out, n); // encrypt input file
    fclose(pub_key_file); // close public key file
    fclose(inp); // close input file
    fclose(out); // close output file
    mpz_clears(n, NULL);
    return 0;
}
