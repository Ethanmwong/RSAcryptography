#include "randstate.h"

#include <stdlib.h>

gmp_randstate_t state;

void randstate_init(uint64_t seed) {
    gmp_randinit_mt(state); // initialize the random state
    gmp_randseed_ui(state, seed); // seed the random state with the given seed
    srandom(seed); // seed the standard random number generator with the given seed
}

void randstate_clear(void) {
    gmp_randclear(state); // free the memory used by the random state
}
