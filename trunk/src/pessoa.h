#ifndef __PESSOA__
#define __PESSOA__

#include <string>

class pessoa
{
    int tp;         // Tipo que vem do Enum tipoPessoa.
    unsigned int k; // Identificador. Forma única de referenciar a pessoa.
    unsigned int b; // Configuração do arquivo possuído pela pessoa.
    unsigned int c; // Cor da pessoa, ou seja, a qual rodada ela pertence.

    double a;       // Tempo de chegada da pessoa no sistema.

    public:
        pessoa(const int tp, const unsigned int c, const double a);

        int tipo() const;
        double chegada() const;
        unsigned int id() const;
        unsigned int blocos() const;
        unsigned int& blocos();
        unsigned int cor() const;

        // Determina quantos blocos faltam para esta pessoa concluir o download.
        unsigned int blocosFaltantes() const;

        // Determina quais blocos esta pessoa pode enviar para a pessoa p.
        unsigned int blocosPossiveis(const pessoa& p) const;

        void viraSeed();    // Quando termina o download, o peer deve virar seed.

        std::string strTipo() const;

        bool operator<(const pessoa& p) const;

        enum tipoPessoa{PUBLISHER, PEER, SEED};

        static unsigned int nextId;         // Forma de criar ids incrementalmente.
        static unsigned int arqCompleto;    // Arquivo completo para referência.
};

#endif

