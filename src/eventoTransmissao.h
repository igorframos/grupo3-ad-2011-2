#ifndef __EVENTO_TRANSMISSAO__
#define __EVENTO_TRANSMISSAO__

#include "evento.h"
#include "pessoa.h"

class eventoTransmissao : public evento
{
    const pessoa* p;        // Pessoa que irá transmitir.
    const unsigned int src; // Id da pessoa que irá transmitir.

    public:
        eventoTransmissao(double t, const pessoa* p);

        const unsigned int id() const;
        pessoa origem() const;
        const pessoa *ptr() const;
};

#endif

