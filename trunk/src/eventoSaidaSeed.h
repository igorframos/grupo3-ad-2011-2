#ifndef __EVENTO_SAIDA_SEED__
#define __EVENTO_SAIDA_SEED__

#include "evento.h"
#include "pessoa.h"
#include <list>

class eventoSaidaSeed : public evento
{
    const pessoa *p;    // Ponteiro para o seed que irá sair.

    public:
        eventoSaidaSeed(double t, const pessoa *p);

        pessoa seed() const;        // Retorna a instância de pessoa.
        const pessoa *ptr() const;  // Retorna o ponteiro para a pessoa.
};

#endif

