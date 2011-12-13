#include "evento.h"

unsigned int evento::nextId;

evento::evento (int tp, double t) : tp(tp), t(t), k(nextId++) {}

evento::~evento() {}

// Um evento deve vir antes de outro em uma ordenação se o tempo for menor.
// Se forem iguais os tempos, escolha o que foi criado primeiro.
bool evento::operator< (const evento& b) const
{
    if (tempo() != b.tempo()) return t < b.tempo();
    return k < b.id();
}

int evento::tipo() const
{
    return tp;
}

unsigned int evento::id() const
{
    return k;
}

double evento::tempo() const
{
    return t;
}

// Apenas para dizer o tipo de evento na forma de string e facilitar impressão
// para debug e afins. Não é utilizado em lugar nenhum agora que terminamos.
std::string evento::strTipo() const
{
    switch (tp)
    {
        case CHEGADA_PEER:
            return "Chegada de peer";
        case TRANSMISSAO:
            return "Transmissão";
        case SAIDA_PEER:
            return "Saída de peer";
        default:
            return "Não reconheci o tipo";
    }
}

