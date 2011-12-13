#include "geradorAleatorio.h"

// A semente padrão é 1.
geradorAleatorio::geradorAleatorio() : seed(1) {}

// Pode-se escolher uma semente diferente de 1.
geradorAleatorio::geradorAleatorio(unsigned int seed) : seed(seed) {}

unsigned int geradorAleatorio::randUniforme()
{
    long long tmp = seed;
    tmp *= MUL;
    tmp %= RANDMAX + 1;
    return seed = (unsigned int) tmp;
}

double geradorAleatorio::randExponencial(double media)
{
    double u = randUniforme() / (double) RANDMAX; 

    return - log(u) * media;
}

