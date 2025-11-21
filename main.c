#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "grafo.h"


int main(){

    int numnodes, from_node, to_node, destino, tipo, capacidade;
    char nome[20];
    float altura, resistencia, resistencia_busca, capacidade_reserva;
    printf("Escolha quantos vertices tera: ");
    scanf("%d", &numnodes);
    getchar();

    float *distancias= (float*)calloc(numnodes, sizeof(float));
    int *predecessorDji= (int*)calloc(numnodes, sizeof(int));
    int *predecessorBFS= (int*)calloc(numnodes, sizeof(int));

    char comando[20];

    Graph *g= create_graph(numnodes);

    printf("Coloque o nome, altura e tipo (1 ou 2) para todos os nos:\n");
    for(int i=0; i<numnodes; i++){
        printf("No %d:\n", i);
        scanf(" %s %f %d", nome, &altura, &tipo);
        set_data(g, i, nome, altura);
        if(tipo == 1){
            g->vertices[i].tipo= TIPO_JUNÇÃO;
        }else if(tipo == 2){
            g->vertices[i].tipo= TIPO_RESERVATORIO;
            scanf("%d", &capacidade_reserva);
            g->vertices[i].dados.reservatorio.capacidade= capacidade_reserva;
        }
    }

    while(1){
        scanf("%s", comando);
        if(strcmp(comando, "sair") == 0){
            free(distancias);
            free(predecessorBFS);
            free(predecessorDji);
            destroy_graph(g);
            break;
        }else if(strcmp(comando, "print") == 0){
            print_graph(g);
        }else if(strcmp(comando, "remover") == 0){
            scanf("%d %d", &from_node, &to_node);
            remove_connect(g, from_node, to_node);
        }else if(strcmp(comando, "tem") == 0){
            scanf("%d %d", &from_node, &to_node);
            if(hasEdge(g, from_node, to_node)){
                printf("Tem conexao entre %d e %d\n", from_node, to_node);
            }else{
                printf("Nao tem conexao entre %d e %d\n", from_node, to_node);
            }
        }else if(strcmp(comando, "adicionar") == 0){
            scanf("%d %d %f %d", &from_node, &to_node, &resistencia, &capacidade);
            add_cano_com_altura(g, from_node, to_node, resistencia, capacidade);
        }else if(strcmp(comando, "dji") == 0){
            printf("Escolha o destino: ");
            scanf("%d", &destino);
            djisktra(g, 0, distancias, predecessorDji);
            imprimir_caminho_pilha(g, destino, predecessorDji);
        }else if(strcmp(comando, "bfs") == 0){
            printf("Escolha o destino: ");
                scanf("%d", &destino);
                BFS(g, 0, predecessorBFS);
                imprimir_caminho_pilha(g, destino, predecessorBFS);
        }else if(strcmp(comando, "analisar") == 0){
            printf("Escolha quem vai ser cortado: ");
            scanf("%d %d", &from_node, &to_node);
            analisar_corte_agua(g, 0, from_node, to_node);
        }else if(strcmp(comando, "dfs") == 0){
            printf("Escolha o destino: ");
            scanf("%d", &destino);
            Arvore *abb= criarABB();
            DFS(g, 0, destino, 1, abb);
            encerrarArvore(abb);
        }else if(strcmp(comando, "prim") == 0){
            prim(g, 0, distancias, predecessorDji);
            imprimir_arestas_prim(g, predecessorDji);
        }else if(strcmp(comando, "buscarResistencia") == 0){
            float min_res, max_res;
            printf("Selecione a Origem, Destino e o Intervalo de Resistencia (Min Max): \n");
            scanf("%d %d %f %f", &from_node, &destino, &min_res, &max_res);
            Arvore *abb= criarABB();
            DFS(g, from_node, destino, 2, abb);
            buscarIntervalo(abb, min_res, max_res);
            encerrarArvore(abb);
        }else if(strcmp(comando, "vis") == 0){
            printf("Escolha o Destino para o Fluxo: ");
            scanf("%d", &destino);
            zerar_fluxos(g);
            int fluxo_total= ford_fukerson(g, 0, destino, 0);
            djisktra(g, 0, distancias, predecessorDji);
            BFS(g, 0, predecessorBFS);
            exportar_json(g, predecessorDji, predecessorBFS, destino, fluxo_total);
        }else if(strcmp(comando, "fuk") == 0){
            printf("Escolha o destino: ");
            scanf("%d", &destino);
            zerar_fluxos(g);
            ford_fukerson(g, 0, destino, 1);
        }
    }



    return 0;
}
