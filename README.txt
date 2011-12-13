#############################################################
#               Avaliação e Desempenho                      #
#                       2011/2                              #
#                                                           #
#               Trabalho de Simulação                       #
#       Implementação e análise de um simulador             #
#              de uma rede peer-to-peer                     #
#                                                           #
#               Gabriel Pires da Silva                      #
#               Igor da Fonseca Ramos                       #
#               Renan da Costa Garrot                       #
#                                                           #
#############################################################


1. Utilização do simulador

    1.1) Compilar com o GCC utilizando o Makefile na raiz do
    projeto.

    1.2) Editar o arquivo 'cenarios.txt' de acordo com o pa-
    drão exibido em seu final. A versão atual é a mais recen-
    te que utilizamos para os testes.
    
    1.3) Para executar, escolha se deseja apenas gerar os re-
    sultados ou se deseja a criação de gráficos.

        a) 'make run' gera um arquivo 'resultados.txt' na ra-
        iz do projeto com dados sobre a simulação (médias,
        intervalos de confiança, etc.). Adicionalmente, são
        criados diversos outros arquivos na pasta log/ que
        contêm informações adicionais necessárias para gerar
        os gráficos. É possível utilizar o gnuplot para pro-
        duzir estes gráficos, chamando 'gnuplot
        log/plotInfo.txt' a partir da pasta raiz do projeto.
        Observe que é necessário executar 'rm log/*Vazao.txt'
        antes de executar o programa novamente para que os
        gráficos vazão x pessoas no sistema sejam produzidos
        corretamente.

        b) 'make graph' produz o mesmo resultado que o coman-
        do 'make run', mas chama automaticamente o gnuplot e
        também apaga os arquivos necessários.

        c) 'make vrun' e 'make vgraph' comportam-se como os
        anteriores, mas imprimem, no console, resultados ex-
        tras a cada rodada de simulação.

2. Relatório

    Para produzir o relatório, execute 'make relatorio'. Na
    pasta 'Relatório', será criado o arquivo em formato pdf.

