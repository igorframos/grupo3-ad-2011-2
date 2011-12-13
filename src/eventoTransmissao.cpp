#include "eventoTransmissao.h"

eventoTransmissao::eventoTransmissao(double t, const pessoa* p) : evento(evento::TRANSMISSAO, t), p(p), src(p->id()) {}

pessoa eventoTransmissao::origem() const
{
    return *p;
}

const pessoa *eventoTransmissao::ptr() const
{
    return p;
}

const unsigned int eventoTransmissao::id() const
{
    return src;
}

