#include "eventoSaidaSeed.h"

eventoSaidaSeed::eventoSaidaSeed(double t, const pessoa *p) : evento(evento::SAIDA_PEER, t), p(p) {}

pessoa eventoSaidaSeed::seed() const
{
    return *p;
}

const pessoa *eventoSaidaSeed::ptr() const
{
    return p;
}

