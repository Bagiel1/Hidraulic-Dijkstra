#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "grafo.h"


int main(){

    int numnodes, from_node, to_node, destino, tipo;
    char nome[20];
    float altura, resistencia, capacidade, resistencia_busca;
    printf("Escolha quantos vertices tera: ");
    scanf("%d", &numnodes);

    float *distancias= (float*)calloc(numnodes, sizeof(float));
    int *predecessorDji= (int*)calloc(numnodes, sizeof(int));
    int *predecessorBFS= (int*)calloc(numnodes, sizeof(int));

    char comando[20];

    Graph *g= create_graph(numnodes);
    Arvore *abb= criarABB();

    printf("Coloque o nome, altura e tipo (1 ou 2) para todos os nos:\n");
    for(int i=0; i<numnodes; i++){
        printf("No %d:\n", i);
        scanf("%s %f %d", nome, &altura, &tipo);
        set_data(g, i, nome, altura);
        if(tipo == 1){
            g->vertices[i].tipo= TIPO_JUNÇÃO;
        }else if(tipo == 2){
            g->vertices[i].tipo= TIPO_RESERVATORIO;
            scanf("%f", &capacidade);
            g->vertices[i].dados.reservatorio.capacidade= capacidade;
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
            scanf("%d %d %f", &from_node, &to_node, &resistencia);
            add_cano_com_altura(g, from_node, to_node, resistencia);
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
            DFS(g, 0, destino, 1, abb);
        }else if(strcmp(comando, "prim") == 0){
            prim(g, 0, distancias, predecessorDji);
        }else if(strcmp(comando, "buscarResistencia") == 0){
            printf("Selecione o Destino e a Resistencia buscada: ");
            scanf("%d", &destino, &resistencia_busca);
            DFS(g, 0, destino, 2, abb);
            No *busca= buscar(abb, resistencia_busca);
            int *caminho= get_caminho_do_no(busca);
            int tamanho= get_tamanho_do_no(busca);

            for(int i=0; i<tamanho; i++){
                printf("%d", caminho[i]);
                if(i < tamanho - 1){
                    printf(" -> ");
                }
            }
        }
    }


    return 0;
}
