#ifndef __GERADOR_ALEATORIO__
#define __GERADOR_ALEATORIO__

#include <cmath>

class geradorAleatorio
{
    private:
        unsigned int seed;      // Semente da simulação.
        
        // Multiplicador para geração do próximo número pseudoaleatório.
        const static unsigned int MUL = 1000003; 

    public:
        // Maior número que pode ser gerado aleatoriamente. 1073741789 é primo.
        const static unsigned int RANDMAX = 1073741789 - 1;

        geradorAleatorio();                     // Construtor sem semente.
        geradorAleatorio(unsigned int seed);    // Construtor com semente.
        unsigned int randUniforme();            // Gera número pseudoaleatório de uma
                                                // distribuição discreta uniforme.
        double randExponencial(double media);   // Gera número pseudoaleatório de uma
                                                // distibuição exponencial.
};

#endif

