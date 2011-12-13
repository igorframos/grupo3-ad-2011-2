#include "include.h"

int main(int argc, char *argv[])
{
    // Abre arquivo de cenários que vem no argv. Se rodar com o makefile é automático.
    FILE* cenarios = fopen(argv[1], "r");
    if (!cenarios)
    {
        printf ("%s\n", argv[1]);
        printf ("Não deu pra abrir os cenários.\n");
        return 1;
    }

    bool verbose = false;
    if (argc > 2 && !strcmp(argv[2], "-v")) verbose = true;

    // Arquivo para onde vão os resultados no final da simulação.
    FILE* resultados = fopen("resultados.txt", "w");
    FILE* plot = fopen("log/plotInfo.txt", "w");

    std::map<std::string,int> reg;  // Contador do número de registros em um arquivo.

    fprintf (plot, "set termoption enhanced\nset encoding utf8\nset key horiz\nset key outside\nset key bot center\nset terminal png size 1024,768\n");

    while (1)
    {
        unsigned int tInicial;          // Momento de início do cenário.

        unsigned int cenario;           // O cenário da simulação.
        unsigned int arquivo;           // O arquivo completo.
        double lambda;                  // A taxa de chegada de peers.
        double mu;                      // A taxa de upload de peers e seeds.
        double U;                       // A taxa de upload do publisher.
        double gamma;                   // 1 / gamma é o tempo médio de permanência de um seed.
        double pRec;                    // Probabilidade de haver recomendação.
        unsigned int populacaoInicial;  // Número de peers inicialmente no sistema.
        char politicaPeer;              // Política de escolha de peer.
        int pPeer;                      // Código interno da política de escolha de peer.
        char politicaBloco;             // Política de escolha de bloco.
        int pBloco;                     // Código interno da política de escolha de bloco.
        unsigned int arqInicial;        // Arquivo que os peers presentes no sistem têm.
        unsigned int numBlocos;         // Número de blocos do arquivo.

        int ba; // Variável só para tirar o warning de valor de retorno ignorado.

        ba = fscanf (cenarios, "%u", &cenario); ++ba;
        if (!cenario) break;

        ba = fscanf (cenarios, "%x %lf %lf", &arquivo, &lambda, &mu); ++ba;
        ba = fscanf (cenarios, "%lf %lf %lf %d", &U, &gamma, &pRec, &populacaoInicial); ++ba;
        ba = fscanf (cenarios, " %c %c %u", &politicaPeer, &politicaBloco, &arqInicial); ++ba;

        if (politicaPeer == 'r') pPeer = simulador::RANDOM_PEER;
        else if (politicaPeer == 'o') pPeer = simulador::OLDEST_PEER;
        else if (politicaPeer == 'n') pPeer = simulador::NEWEST_PEER;
        else if (politicaPeer == 'm') pPeer = simulador::NEWNEWEST_PEER;
        if (politicaBloco == 'r') pBloco = simulador::RANDOM_PIECE;
        else pBloco = simulador::RAREST_FIRST;

        // Inicializações de variáveis de classe do tipo pessoa.
        evento::nextId = 0;
        pessoa::nextId = 0;
        pessoa::arqCompleto = arquivo;

        numBlocos = __builtin_popcount(arquivo);

        char arqOut[64];
        char plotTitle[128];

        if (cenario <= 2)
        {
            sprintf (arqOut, "log/cen%dlam%.1f.txt", cenario, lambda);
            sprintf (plotTitle, "Cenário %d - λ = %.1f", cenario, lambda); 
        }
        else if (cenario == 3)
        {
            sprintf (arqOut, "log/cen%dini%03d.txt", cenario, populacaoInicial);
            sprintf (plotTitle, "Cenário %d - População inicial: %d", cenario, populacaoInicial);
        }
        else
        {
            sprintf (arqOut, "log/cen%db%02dini%02dai%d%c%c.txt", cenario, numBlocos, populacaoInicial, __builtin_popcount(arqInicial), politicaPeer, politicaBloco);
            sprintf (plotTitle, "Cenário %d - Blocos: %d - População inicial: %d - Condição inicial: %d bloco - Políticas: %c & %c", cenario, numBlocos, populacaoInicial, __builtin_popcount(arqInicial), politicaPeer, politicaBloco);
        }

        fprintf (plot, "set title \"%s\"\nset xlabel \"Chegadas\"\n", plotTitle);
        fprintf (plot, "set output \"%s.png\"\nplot \"%s\" u 1:2 title \"Permanência\" with lines lw 2, \"%s\" u 1:3 title \"Download\" with lines lw 2, \"%s\" u 1:4 title \"N\" with lines lw 2, \"%s\" u 1:5 title \"Vazão\" with lines lw 2, \"%s\" u 1:6 title \"Peers\" with lines lw 2\n", arqOut, arqOut, arqOut, arqOut, arqOut, arqOut);
        
        printf ("Começarei o cenário %d com arquivo %x, lambda %.1f, mu %.1f, U %.1f, gamma %.1f, população inicial de %d, arquivo inicial %x e políticas %c (peer) %c (bloco)\n", cenario, arquivo, lambda, mu, U, gamma, populacaoInicial, arqInicial, politicaPeer, politicaBloco);

        // O cenário começa aqui, então armazenaremos o momento de início.
        tInicial = time(NULL);

        // Chama o construtor do simulador de um cenário.
        simulador f(1/lambda, 1/mu, 1/gamma, 1/U, pRec, pPeer, pBloco, populacaoInicial, arqInicial, arqOut);

        // Essas são as variáveis usadas nos cálculos de intervalos de confiança.
        // var tem a soma das rodadas, var2 a soma dos quadrados das rodadas,
        // L é o limite inferior do intervalo de confiança,
        // U é o limite superior do intervalo de confiança,
        // p é o tamanho do intervalo de confiança como porcentagem do estimador da média.
        double tempoDownload = 0, tempoDownload2 = 0, Ld, Ud, pd;
        double vazao = 0, vazao2 = 0, Lv, Uv, pv;
        double pessoas = 0, pessoas2 = 0, Ln, Un, pn;
        double peers = 0, peers2 = 0, Lp, Up, pp;
        double tempo = 0, tempo2 = 0, Lt, Ut, pt;
        std::vector<double> tempoPorN, tempoPorN2, LpN, UpN, ppN;
        std::vector<std::vector<double> > temposDownload;
        int n = 0;  // Número de rodadas.

        while (f.haEvento())
        {
            f.trataProximoEvento();

            if (!f.fimRodada()) continue;

            ++n;

            if (verbose) printf ("Rodada %d\n", n);

            // Recupera os dados da simulação de uma rodada.
            double mediaDownload = f.mediaDownload();
            double mediaVazao = f.mediaVazao();
            double mediaN = f.mediaPessoas();
            double mediaPeers = f.mediaPeers();
            double mediaT = f.mediaPermanencia();

            // Só é preciso fazer a CDF de T nos cenários 1 e 2, então só pegamos esses dados neste cenário.
            if (cenario == 1 || cenario == 2) temposDownload.push_back(f.temposDeDownload());

            tempoDownload += mediaDownload;
            tempoDownload2 += mediaDownload * mediaDownload;
            vazao += mediaVazao;
            vazao2 += mediaVazao * mediaVazao;
            pessoas += mediaN;
            pessoas2 += mediaN * mediaN;
            peers += mediaPeers;
            peers2 += mediaPeers * mediaPeers;
            tempo += mediaT;
            tempo2 += mediaT * mediaT;

            // Só é preciso fazer a pmf de N no cenário 1, então só pegamos esses dados nesse cenário.
            if (cenario == 1)
            {
                std::vector<double> tempoN = f.tempoPorN();
                for (int i = 0; i < (int) tempoN.size(); ++i)
                {
                    if ((int) tempoPorN.size() <= i)
                    {
                        tempoPorN.push_back(tempoN[i]);
                        tempoPorN2.push_back(tempoN[i] * tempoN[i]);
                    }
                    else
                    {
                        tempoPorN[i] += tempoN[i];
                        tempoPorN2[i] += tempoN[i] * tempoN[i];
                    }
                }
            }

            // Com menos de duas rodadas nem podemos calcular o intervalo de confiança.
            if (n < 2) continue;

            // Os resultados só valem com 30 rodadas ou mais, que é quando a distribuição t-Student
            // se aproxima o suficiente da distribuição normal.
            bool encerra = (n >= 100);

            if (cenario == 1)
            {
                while (LpN.size() < tempoPorN.size()) LpN.push_back(0);
                while (UpN.size() < tempoPorN.size()) UpN.push_back(0);
                while (ppN.size() < tempoPorN.size()) ppN.push_back(0);

                for (int i = 0; i < (int) tempoPorN.size(); ++i)
                {
                    double mu = tempoPorN[i] / n;
                    double sigma = sqrt((tempoPorN2[i] - 2 * mu * tempoPorN[i] + n * mu * mu) / (n - 1));
                    LpN[i] = mu - 1.96 * sigma / sqrt(n);
                    UpN[i] = mu + 1.96 * sigma / sqrt(n);
                    ppN[i] = 100 * 1.96 * sigma / (mu * sqrt(n));
                }
            }

            double mu = tempoDownload / n;
            double sigma = sqrt((tempoDownload2 - 2 * mu * tempoDownload + n * mu * mu) / (n - 1));
            Ld = mu - 1.96 * sigma / sqrt(n);
            Ud = mu + 1.96 * sigma / sqrt(n);
            pd = 100 * 1.96 * sigma / (mu * sqrt(n));

            if (pd > 10)
            {
                if (encerra) printf ("Tempo de Download não estabilizou.\n");
                encerra = false;
            }

            mu = vazao / n;
            sigma = sqrt((vazao2 - 2 * mu * vazao + n * mu * mu) / (n - 1));
            Lv = mu - 1.96 * sigma / sqrt(n);
            Uv = mu + 1.96 * sigma / sqrt(n);
            pv = 100 * 1.96 * sigma / (mu * sqrt(n));

            if (pv > 10)
            {
                if (encerra) printf ("Vazão não estabilizou.\n");
                encerra = false;
            }

            mu = pessoas / n;
            sigma = sqrt((pessoas2 - 2 * mu * pessoas + n * mu * mu) / (n - 1));
            Ln = mu - 1.96 * sigma / sqrt(n);
            Un = mu + 1.96 * sigma / sqrt(n);
            pn = 100 * 1.96 * sigma / (mu * sqrt(n));

            if (pn > 10)
            {
                if (encerra) printf ("Número de pessoas no sistema não estabilizou.\n");
                encerra = false;
            }

            mu = peers / n;
            sigma = sqrt((peers2 - 2 * mu * peers + n * mu * mu) / (n - 1));
            Lp = mu - 1.96 * sigma / sqrt(n);
            Up = mu + 1.96 * sigma / sqrt(n);
            pp = 100 * 1.96 * sigma / (mu * sqrt(n));

            if (pp > 10)
            {
                if (encerra) printf ("Número de peers no sistema não estabilizou.\n");
                encerra = false;
            }

            mu = tempo / n;
            sigma = sqrt((tempo2 - 2 * mu * tempo + n * mu * mu) / (n - 1));
            Lt = mu - 1.96 * sigma / sqrt(n);
            Ut = mu + 1.96 * sigma / sqrt(n);
            pt = 100 * 1.96 * sigma / (mu * sqrt(n));

            if (pt > 10)
            {
                if (encerra) printf ("Tempo de permanência no sistema não estabilizou.\n");
                encerra = false;
            }

            if (verbose)
            {
                printf ("\tDownload:    %.12f (%.12f, %.12f) %.12f\n", tempoDownload / n, Ld, Ud, pd);
                printf ("\tPermanencia: %.12f (%.12f, %.12f) %.12f\n", tempo / n, Lt, Ut, pt);
                printf ("\tVazão:       %.12f (%.12f, %.12f) %.12f\n", vazao / n, Lv, Uv, pv);
                printf ("\tPessoas:     %.12f (%.12f, %.12f) %.12f\n", pessoas / n, Ln, Un, pn);
                printf ("\tPeers:       %.12f (%.12f, %.12f) %.12f\n", peers / n, Lp, Up, pp);
            }

            if (encerra == true) break;
        }

        printf ("Encerrei o cenário %d com arquivo %x, lambda %.1f, mu %.1f, U %.1f, gamma %.1f, população inicial de %d, arquivo inicial %x e políticas %c (peer) %c (bloco)\n", cenario, arquivo, lambda, mu, U, gamma, populacaoInicial, arqInicial, politicaPeer, politicaBloco);

        // Imprime o tempo de execução do cenário.
        printf ("Tempo de execução do cenário: %u segundos.\n\n", (unsigned int) time(NULL) - tInicial);

        // Daqui em diante é só impressão dos resultados no arquivo de saída.
        double mud = tempoDownload / n;
        double muv = vazao / n;
        double mun = pessoas / n;
        double mup = peers / n;
        double mut = tempo / n;
        fprintf (resultados, "Cenário: %d - Arquivo: %x - lambda: %.1f - População Inicial: %d - Rodadas: %d - Arquivo inicial: %x - Políticas: %c (peer) %c (bloco)\n", cenario, arquivo, lambda, populacaoInicial, n, arqInicial, politicaPeer, politicaBloco);
        fprintf (resultados, "Duração da Fase transiente: %.12f (%u chegadadas)\n", f.fimFaseTransiente(), f.eventosFaseTransiente());
        fprintf (resultados, "Média (Tempo de Download): %.12f - IC: (%.12f, %.12f) - P: %.12f\n", mud, Ld, Ud, pd);
        fprintf (resultados, "Média (Tempo de Permanência): %.12f - IC: (%.12f, %.12f) - P: %.12f\n", mut, Lt, Ut, pt);
        fprintf (resultados, "Média (Vazão): %.12f - IC: (%.12f, %.12f) - P: %.12f\n", muv, Lv, Uv, pv);
        fprintf (resultados, "Média (Pessoas): %.12f - IC: (%.12f, %.12f) - P: %.12f\n", mun, Ln, Un, pn);
        fprintf (resultados, "Média (Peers): %.12f - IC: (%.12f, %.12f) - P: %.12f\n", mup, Lp, Up, pp);

        /*
         * O trecho a seguir tem, por objetivo, verificar a coerência dos resultados impressos acima.
         * Testaremos o resultado de Little para todo o sistema, para os peers e para os seeds.
         * Queremos saber se os resultados encontrados estão dentro daquilo que se consideraria
         * razoável quando comparados entre si.
         */
        if (cenario == 1)
        {
            double rho = lambda / U,
                   analitico;

            // Número de pessoas na fila.
            analitico = rho / (1 - rho);
            if (analitico < Ln || analitico < Lp || analitico > Un || analitico > Up) fprintf (resultados, "Número de pessoas na fila está incoerente. (%.12f %.12f) (%.12f %.12f) %.12f\n", Ln, Un, Lp, Up, analitico);

            // Tempo de download e de permanência.
            analitico = 1 / (U - lambda);
            if (analitico < Lt || analitico < Ld || analitico > Ut || analitico > Ud) fprintf (resultados, "Tempo de download está incoerente. (%.12f %.12f) (%.12f %.12f) %.12f\n", Lt, Ut, Ld, Ud, analitico);

            // Vazão.
            if (lambda < Lv || lambda > Uv) fprintf (resultados, "Vazão está incoerente. %.12f %.12f %.1f\n", Lv, Uv, lambda);
           
            // pmf não está sendo testada.

        }
        else if (cenario >= 4 && cenario <= 6)
        {
            double l, u;    // Comparamos sempre o intervalo de confiança.

            // Tempo de download em relação ao tempo de permanência.
            l = Ld + 1 / gamma;
            u = Ud + 1 / gamma;
            if (l > Ut || u < Lt) fprintf (resultados, "Tempo de download incoerente com tempo de permanência.\n");
            l = Lt - 1 / gamma;
            u = Ut - 1 / gamma;
            if (l > Ud || u < Ld) fprintf (resultados, "Tempo de download incoerente com tempo de permanência.\n");

            // Número de peers, número de pessoas e vazão.
            l = Lp + Lv * 1 / gamma;
            u = Up + Uv * 1 / gamma;
            if (populacaoInicial < l || populacaoInicial > u) fprintf (resultados, "Número de peers, número de pessoas e vazão estão incoerentes. %.12f %.12f %d\n", l, u, populacaoInicial);

            // Little para pessoas
            l = Lt * Lv;
            u = Ut * Uv;
            if (mun < l || mun > u) fprintf (resultados, "Little para número de pessoas e tempo de permanência está incoerente.\n");

            // Little para peers
            l = Ld * Lv;
            u = Ud * Uv;
            if (Up < l || Lp > u) fprintf (resultados, "Little para número de peers e tempo de download está incoerente.\n");

            // Little para seeds
            l = 1 / gamma * Lv;
            u = 1 / gamma * Uv;
            if (Un - Lp < l || Ln - Up > u) fprintf (resultados, "Little para número de seeds está incoerente.\n");
        }
        // Fim do trecho de testes.

        if (cenario == 1)
        {
            fprintf (resultados, "pmf do Número total de usuários no sistema:\n");

            char arqpmf[64];
            sprintf (arqpmf, "log/cen%dlam%.1fpmf.txt", cenario, lambda);
            FILE* arq = fopen(arqpmf, "w");

            double rho = lambda / U;

            for (int i = 0; i < (int) tempoPorN.size(); ++i)
            {
                fprintf (arq, "%u %.12f %.12f %.12f %.12f\n", i, tempoPorN[i] / n, std::max(LpN[i], 0.0), UpN[i], (1-rho)*pow(rho, i));
                fprintf (resultados, "\t%u: %.12f - IC: (%.12f, %.12f) %.12f - analítico: %.12f\n", i, tempoPorN[i] / n, LpN[i], UpN[i], ppN[i], (1-rho)*pow(rho, i));
                if ((1-rho)*pow(rho, i) < LpN[i] || (1-rho)*pow(rho, i) > UpN[i]) printf ("pmf incoerente para %d\n", i);
            }

            fclose(arq);

            fprintf (plot, "set title \"pmf de N: Cenário %d - λ = %.1f\"\nset xlabel \"k\"\nset ylabel \"P(N = k)\"\n", cenario, lambda);
            fprintf (plot, "set output \"%s.png\"\nplot \"%s\" u 1:2:3:4 title \"pmf da Simulação\" with yerrorbars lt 8 lw 2, \"%s\" u 1:5 title \"pmf Analítica\" with points lt 1 lw 2\nunset ylabel\n", arqpmf, arqpmf, arqpmf);

            tempoPorN.clear();
            tempoPorN2.clear();
            LpN.clear();
            UpN.clear();
            ppN.clear();
        }

        if (cenario == 1 || cenario == 2)
        {
            char plotline[1 << 20];
            plotline[0] = 0;

            for (int i = 0; i < n; ++i)
            {
                char arqCDF[64];
                sprintf (arqCDF, "log/cen%dlam%.1fCDF%02d.txt", cenario, lambda, i+1);
                FILE* plotCDF = fopen(arqCDF, "w");

                sort(temposDownload[i].begin(), temposDownload[i].end());

                double p = 1.0 / temposDownload[i].size();
                for (int j = 0; j < (int) temposDownload[i].size(); ++j, p += 1.0 / temposDownload[i].size())
                {
                    fprintf (plotCDF, "%.12f %.12f\n", temposDownload[i][j], p);
                }

                fclose(plotCDF);

                char tmp[64];
                sprintf (tmp, "\"%s\" title \"Rodada %d\" with lines,", arqCDF, i+1);

                strcat(plotline, tmp);
            }
            temposDownload.clear();

            plotline[strlen(plotline) - 1] = 0;

            fprintf (plot, "set title \"CDF: tempo de download - Cenário %d - λ = %.1f\"\nset xlabel \"x\"\nset ylabel \"P(D < x)\"\n", cenario, lambda);
            fprintf (plot, "set output \"log/cen%dlam%.1fCDF.png\"\nplot %s\nunset ylabel\n", cenario, lambda, plotline);
        }

        if (cenario >= 3 && cenario <= 6 && arqInicial == 0)
        {
            char nome[64];
            sprintf (nome, "log/cen%db%02d%c%cVazao.txt", cenario, numBlocos, politicaPeer, politicaBloco);
            FILE* arq = fopen(nome, "a");

            reg[nome]++;
            if (reg[nome] <= 50)
            {
                fprintf (arq, "%d %.12f %.12f %.12f\n", populacaoInicial, muv, Lv, Uv);
                fprintf (plot, "set title \"Vazão média: Cenário %d - Blocos: %d - Políticas: %c (peer) & %c (bloco)\"\nset xlabel \"Pessoas no sistema\"\n", cenario, numBlocos, politicaPeer, politicaBloco);
                fprintf (plot, "set output \"%s.png\"\nplot \"%s\" u 1:2:3:4 title \"Vazão média\" with yerrorbars lt 8 lw 2\n", nome, nome);
            }

            fclose(arq);
        }

        fprintf (resultados, "\n");
    }

    fclose(resultados);
    fclose(plot);

    return 0;
}

