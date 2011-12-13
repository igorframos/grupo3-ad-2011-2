#include "pessoa.h"

unsigned int pessoa::nextId;
unsigned int pessoa::arqCompleto;

pessoa::pessoa(const int tp, const unsigned int c, const double a) : tp(tp), k(nextId++), c(c), a(a)
{
    if (tp == PUBLISHER || tp == SEED)
    {
        b = arqCompleto;
    }
    else if (tp == PEER)
    {
        b = 0;
    }
}

unsigned int pessoa::id() const
{
    return k;
}

int pessoa::tipo() const
{
    return tp;
}

unsigned int pessoa::blocos() const
{
    return b;
}

unsigned int& pessoa::blocos()
{
    return b;
}

unsigned int pessoa::cor() const
{
    return c;
}

double pessoa::chegada() const
{
    return a;
}

unsigned int pessoa::blocosPossiveis(const pessoa& p) const
{
    return (b ^ p.blocos()) & b;
}

unsigned int pessoa::blocosFaltantes() const
{
    return __builtin_popcount(b ^ arqCompleto);
}

// Virar seed significa apenas alterar o tipo. Todo o resto é responsabilidade de quem
// gerencia as listas de peers e seeds e as transmissões.
void pessoa::viraSeed()
{
    tp = SEED;
}

// Comparação se dá apenas pelo id.
bool pessoa::operator<(const pessoa& p) const
{
    return id() < p.id();
}

// Apenas para dizer o tipo de pessoa na forma de string e facilitar impressão
// para debug e afins. Não é utilizado em lugar nenhum agora que terminamos.
std::string pessoa::strTipo() const
{
    switch(tp)
    {
        case PUBLISHER:
            return "Publisher";
        case PEER:
            return "Peer";
        case SEED:
            return "Seed";
        default:
            return "Não reconheci o tipo";
    }
}

