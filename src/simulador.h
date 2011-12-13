#ifndef __FILA_EVENTOS__
#define __FILA_EVENTOS__

#include <set>
#include <list>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "pessoa.h"
#include "evento.h"
#include "eventoChegadaPeer.h"
#include "eventoSaidaSeed.h"
#include "eventoTransmissao.h"
#include "geradorAleatorio.h"

class simulador
{
    // Atenção: letras correspondentes a uma taxa são, na verdade, a média (1 / taxa).
    // Isso facilita na hora de tratar casos como gamma = infinito.
    double lambda,      // Taxa de chegadas de peers.
           mu,          // Taxa de upload dos peers e seeds.
           gamma,       // 1 / gamma é o tempo médio de permanência de um seed.
           U,           // Taxa de upload do publisher.
           pRec,        // Probabilidade de recomendação.
           tAtual;      // Tempo atual de simulação.
    geradorAleatorio g;
    std::set<std::pair<double,evento*> > fila;  // Fila de eventos.
    pessoa publisher;
    std::list<pessoa> peers;
    std::list<pessoa> seeds;
    std::set<int> setPeers;
    std::set<int> setSeeds;
    int pPeer, pBloco;                  // Políticas de escolha de peer e bloco.
    double T, T0, T1,                   // Tempo médio de permanência no sistema.
           D, D0, D1,                   // Tempo médio de download.
           A, A0, A1,                   // Área do gráfico Pessoas no sistema x Tempo.
           V, V0, V1,                   // Vazão média do sistema.
           P, P0, P1;                   // Área do gráfico Peers no sistema x Tempo.
    double t,                           // Último tempo em que foram computadas áreas. 
           tTotal,                      // Tempo total da rodada.
           tRodada,                     // Momento de início da rodada.
           fimTrans;                    // Tempo até o fim da fase transiente do cenário.
    unsigned int f;                     // Rodada atual.
    unsigned int n;                     // Número de chegadas total.
    unsigned int maxBlocos;             // Número de blocos do arquivo.
    unsigned int saidas,                // Total de saídas na rodada.
                 totalSaidas,           // Total de saídas ao longo de toda a simulação.
                 chegadas,              // Total de chegadas na rodada.
                 saidasComputadas,      // Saídas de seeds da cor da rodada.
                 downloadsConcluidos,   // Downloads concluídos por peers da cor da rodada.
                 downloadsTotais,       // Todos os downloads da simulação.
                 eventosFimTrans;       // Número de chegadas e saídas até o fim da fase transiente.
    FILE* out;                          // Onde gravar dados sobre fase transiente.
    std::vector<unsigned int> possuem;  // Número de pessoas que possuem cada bloco excluindo o publisher.
    std::vector<double> tempoN;         // Tempo em que o sistema esteve com um determinado número de pessoas.

    bool fimDeRodada;                   // Determina se a rodada acabou para repassar.
    std::vector<double> tDownloads;     // Todos os tempos de download da rodada.
    std::vector<double> saidaTempoN;    // Tempo em que o sistema esteve com um determinado número de pessoas para repassar.

    void agendaChegadaPeer(double t);
    void agendaSaidaSeed(double t, const pessoa& p);
    void agendaTransmissao(double t, const pessoa& p);

    void insereEvento(evento* e);

    void trataChegadaPeer(const eventoChegadaPeer& e);
    void trataSaidaSeed(const eventoSaidaSeed& e);
    void trataTransmissao(const eventoTransmissao& e);

    void testaFimRodada();

    std::list<pessoa>::iterator escolhePeer(const pessoa& origem);
    unsigned int escolheBloco(const pessoa& origem, const pessoa& destino);

    public:
        simulador(double lambda, double mu, double gamma, double U, double pRec, int pPeer, int pBloco, int peersIniciais, unsigned int arqInicial, char arqOut[64]);
        ~simulador();

        bool haEvento();
        void trataProximoEvento();
        unsigned int fase();
        unsigned int pessoasNoSistema();
        unsigned int usuarios();
        unsigned int peersNoSistema();
        unsigned int chegadasTotais();
        unsigned int saidasTotais();

        std::vector<double> tempoPorN();
        std::vector<double> temposDeDownload();
        bool fimRodada();
        double mediaVazao();
        double mediaPeers();
        double mediaPessoas();
        double mediaDownload();
        double mediaPermanencia();
        double fimFaseTransiente();
        unsigned int eventosFaseTransiente();

        const static unsigned int TRANSIENTE = 0;       // Código da fase transiente.
        const static unsigned int DELTA = 500;          // Número de chegadas até um teste do fim da fase transiente.
        const static unsigned int TAMTRANSMIN = 5000;   // Tamanho mínimo da fase transiente.
        const static unsigned int TAMRODADA = 10000;    // Número de chegadas em uma rodada.
        const static double EPS = 0.025;                // Diferença máxima entre duas médias para considerarmos a fase transiente terminada.

        enum politicas{RANDOM_PEER, OLDEST_PEER, NEWEST_PEER, NEWNEWEST_PEER, RANDOM_PIECE, RAREST_FIRST};
};

#endif

