#ifndef __EVENTO__
#define __EVENTO__

#include <string>

class evento
{
    protected:
        int tp;         // Tipo de evento. (Valor vem do ENUM tipoEvento.)
        double t;       // Tempo de ocorrência do evento.
        unsigned int k; // Identificador do evento. Uma precaução até excessiva para que
                        // eventos diferentes não sejam avaliados como iguais pelo
                        // operador <.

    public:
        evento(int tp, double t);
        virtual ~evento();

        bool operator<(const evento& b) const;
 
        int tipo() const;
        double tempo() const;
        unsigned int id() const;
        std::string strTipo() const;

        enum tipoEvento{CHEGADA_PEER, SAIDA_PEER, TRANSMISSAO};

        static unsigned int nextId; // Forma de criar os id de forma incremental.
};

#endif

