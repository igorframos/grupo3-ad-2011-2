#ifndef __EVENTO_CHEGADA_PEER__
#define __EVENTO_CHEGADA_PEER__

#include "evento.h"

class eventoChegadaPeer : public evento
{
    // Evento de chegada de peer não guarda nenhuma informação
    // adicional. Só é preciso saber o momento em que o próximo
    // peer chegará e isto é um campo da classe pai evento.

    public:
        eventoChegadaPeer(double t);
};

#endif

