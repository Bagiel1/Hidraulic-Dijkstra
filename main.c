#include <stdlib.h>
#include <stdio.h>
#include "grafo.h"


int main(){

    int NUM_NOS = 6;
    int ORIGEM = 0;
    int DESTINO = 4; 

    Graph *g = create_graph(NUM_NOS);

    printf("--- Configurando Nós ---\n");
    set_data(g, 0, "CaixaDAgua", 10.0);
    set_data(g, 1, "Juncao_A", 8.0);
    set_data(g, 2, "Juncao_B", 7.0);
    set_data(g, 3, "Torneira_1", 1.0);
    set_data(g, 4, "Torneira_2", 0.5);
    set_data(g, 5, "Ponto_Morto", 6.0); 


    g->vertices[0].tipo = TIPO_RESERVATORIO;
    g->vertices[0].dados.reservatorio.capacidade = 1000.0;

    g->vertices[1].tipo = TIPO_JUNÇÃO;
    g->vertices[2].tipo = TIPO_JUNÇÃO;
    g->vertices[3].tipo = TIPO_JUNÇÃO;
    g->vertices[4].tipo = TIPO_JUNÇÃO;
    g->vertices[5].tipo = TIPO_JUNÇÃO;


    // --- 3. CONFIGURAÇÃO DOS CANOS (ARESTAS) ---
    // Usando a tua função de gravidade
    printf("\n--- Adicionando Canos (com Gravidade) ---\n");
    
    // Caminho A (mais resistente): 0->1->3 (Custo: 1.0 + 6.0 = 7.0)
    add_cano_com_gravidade(g, 0, 1, 1.0);
    add_cano_com_gravidade(g, 1, 3, 6.0);

    // Caminho B (menos resistente): 0->2->3 (Custo: 4.0 + 2.0 = 6.0)
    add_cano_com_gravidade(g, 0, 2, 4.0);
    add_cano_com_gravidade(g, 2, 3, 2.0);

    // O "desvio" que o Dijkstra vai achar:
    // 0->1->2->3 (Custo: 1.0 + 2.0 + 2.0 = 5.0)
    add_cano_com_gravidade(g, 1, 2, 2.0); 

    // Continuação do caminho
    add_cano_com_gravidade(g, 3, 4, 1.5); 

    // Cano para o Ponto Morto (para o teste de corte)
    add_cano_com_gravidade(g, 0, 5, 1.0);

    // Imprime o estado inicial
    // (Nota: o teu print_graph não está no formato .dot,
    //  mas vai mostrar os dados que tu mandaste)
    // print_graph(g); 


    // --- 4. TESTE: DIJKSTRA vs BFS ---
    printf("\n--- Comparação de Algoritmos (Origem: %d, Destino: %d) ---\n", ORIGEM, DESTINO);

    // Aloca os mapas (um para cada algoritmo)
    float *dist_dijkstra = (float*)calloc(g->numnodes, sizeof(float));
    int   *pred_dijkstra = (int*)calloc(g->numnodes, sizeof(int));
    int   *pred_bfs      = (int*)calloc(g->numnodes, sizeof(int));
    
    // Roda os Algoritmos
    djisktra(g, ORIGEM, dist_dijkstra, pred_dijkstra);
    BFS(g, ORIGEM, pred_bfs);

    // Imprime os Resultados do Dijkstra
    printf("1. Dijkstra (Menor Resistência):\n");
    printf("  - Custo Total para %d: %.2f\n", DESTINO, dist_dijkstra[DESTINO]);
    imprimir_caminho_com_pilha(g, DESTINO, pred_dijkstra);

    // Imprime os Resultados do BFS
    printf("\n2. BFS (Menos Canos):\n");
    imprimir_caminho_com_pilha(g, DESTINO, pred_bfs);


    // --- 5. TESTE: ANÁLISE DE CORTE DE ÁGUA ---
    // Vamos cortar o abastecimento do Ponto Morto
    
    analisar_corte_de_agua(g, ORIGEM, 0, 5);


    // --- 6. LIMPEZA TOTAL ---
    printf("\n--- Encerrando e Limpando Memória ---\n");
    free(dist_dijkstra);
    free(pred_dijkstra);
    free(pred_bfs);
    destroy_graph(g);
    
    printf("Tudo limpo. Fim do teste.\n");
    return 0;
}
