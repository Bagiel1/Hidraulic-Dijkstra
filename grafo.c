#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <float.h>
#include <limits.h>
#include "grafo.h"
#include "pq.h"
#include "fila.h"
#include "pilha.h"
#include "abb.h"

#ifndef max
    #define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

Graph *create_graph(int numnodes){
    Graph *g= (Graph*)malloc(sizeof(Graph));
    if(g == NULL){
        return NULL;
    }

    g->numnodes= numnodes;
    g->list_adj= calloc(g->numnodes,sizeof(Cano*));
    g->vertices= calloc(sizeof(Vertice), g->numnodes);
    if(g->list_adj == NULL || g->vertices == NULL){
        free(g->vertices);
        free(g->list_adj);
        free(g);
        return NULL;
    }
    
    for(int i=0; i<numnodes; i++){
        g->vertices[i].id= i;
        g->vertices[i].nome= NULL;
    }

    return g;

}


void remove_connect(Graph *g, int from_node, int to_node){
    Cano *cano= g->list_adj[from_node];
    Cano *cano_aux= NULL;
    while(cano != NULL){
        if(cano->destino == to_node){
            if(cano_aux == NULL){
                g->list_adj[from_node]= cano->proximo;
            }else{
                cano_aux->proximo= cano->proximo;
            }
            free(cano);
            return;
        }
        cano_aux= cano;
        cano= cano->proximo;
    }
    printf("Não encontrado essa ligacao");
}

void destroy_graph(Graph *g){
    if(g == NULL){
        return;
    }

    if(g->list_adj != NULL){
        for(int i=0; i<g->numnodes; i++){
            Cano *cano= g->list_adj[i];
            while(cano != NULL){
                Cano *cano_liberar= cano;
                cano= cano->proximo;
                free(cano_liberar);
            }
        }
        free(g->list_adj);
    }

    if(g->vertices != NULL){
        for(int i=0; i<g->numnodes; i++){
            if(g->vertices[i].nome != NULL){
                free(g->vertices[i].nome);
            }
        }
        free(g->vertices);
    }
    
    free(g);
}

void print_graph(Graph *g){
    if(g == NULL) return;

    printf("digraph {\n");

    for(int i=0; i<g->numnodes; i++){
        char *nome= g->vertices[i].nome ? g->vertices[i].nome : "SemNome";
        printf("%d nome: %s altura: %.1f\n", i, nome, g->vertices[i].altura);
    }

    printf("\n");

    for(int from=0; from<g->numnodes; from++){
        Cano *cano= g->list_adj[from];
        while(cano != NULL){
            if(cano->eh_reversa == true){
                cano= cano->proximo;
                continue;
            }
            printf("%d -> %d, resistencia: %.1f, capacidade: %d\n", from, cano->destino, cano->resistencia, cano->capacidade);
            cano= cano->proximo;
        }
    }
    printf("}\n");
}

int set_data(Graph *g, int id, char *nome, float altura){
    if(g == NULL || id < 0 || id >= g->numnodes){
        return 0;
    }

    Vertice *v= &g->vertices[id];
    v->altura= altura;

    if(v->nome != NULL){
        free(v->nome);
        v->nome= NULL;
    }

    v->nome= (char*)malloc(strlen(nome) + 1);
    if(v->nome == NULL){
        return 0;
    }
    
    strcpy(v->nome, nome);
    return 1;
}

int add_edge(Graph *g, int from_node, int to_node, float resistencia, int capacidade, bool eh_reversa){

    if(g==NULL || from_node<0 || from_node>=g->numnodes || to_node < 0 || to_node >= g->numnodes){
        return 0;
    }
    if(hasEdge(g, from_node, to_node)){
        return 0;
    }

    
    Cano *cano_novo= (Cano*)calloc(1, sizeof(Cano));
    cano_novo->destino= to_node;
    cano_novo->proximo= g->list_adj[from_node];
    cano_novo->resistencia= resistencia;
    cano_novo->eh_reversa= eh_reversa;
    cano_novo->fluxo= 0;
    cano_novo->capacidade= capacidade;
    g->list_adj[from_node]= cano_novo;
    
    return 1;
}

int hasEdge(Graph *g, int from_node, int to_node){
    Cano *cano= g->list_adj[from_node];
    while(cano != NULL){
        if(cano->eh_reversa == true){
            cano= cano->proximo;
            continue;
        
        }
        if(cano->destino == to_node){
            return 1;
        }
        cano= cano->proximo;
    }
    return 0;
}

void add_cano_com_altura(Graph *g, int id_A, int id_B, float resistencia, int capacidade){
    float altura_A= g->vertices[id_A].altura;
    float altura_B= g->vertices[id_B].altura;

    if(altura_A > altura_B){
        float resistencia_final= resistencia;
        if(g->vertices[id_B].tipo == TIPO_RESERVATORIO){
            resistencia_final += g->vertices[id_B].dados.reservatorio.capacidade * 0.001;
        }
        add_edge(g, id_A, id_B, resistencia_final, capacidade, false);
        add_edge(g, id_B, id_A, resistencia_final, 0, true);
    }else if(altura_B > altura_A){
        float resistencia_final= resistencia;
        if(g->vertices[id_A].tipo == TIPO_RESERVATORIO){
            resistencia_final += g->vertices[id_A].dados.reservatorio.capacidade * 0.001;
        }
        add_edge(g, id_B, id_A, resistencia_final, capacidade, false);
        add_edge(g, id_A, id_B, resistencia_final, 0, true);
    }else{
        float resistencia_final1= resistencia;
        float resistencia_final2= resistencia;
        if(g->vertices[id_A].tipo == TIPO_RESERVATORIO){
            resistencia_final2 += g->vertices[id_A].dados.reservatorio.capacidade * 0.001;
        }
        if(g->vertices[id_B].tipo == TIPO_RESERVATORIO){
            resistencia_final1 += g->vertices[id_B].dados.reservatorio.capacidade * 0.001;
        }
        add_edge(g, id_A, id_B, resistencia_final1, capacidade, false);
        add_edge(g, id_B, id_A, resistencia_final2, capacidade, false);
    }
}

void djisktra(Graph *g, int origem, float *distancias, int *predecessor){
    FilaPrio *pq= create_pq(g->numnodes * g->numnodes);
    bool *visitados= (bool*)calloc(g->numnodes, sizeof(bool));

    for(int i=0; i<g->numnodes; i++){
        distancias[i]= FLT_MAX;
        predecessor[i]= -1;
        visitados[i]= false;
    }
    distancias[origem]= 0.0;
    insere(pq, origem, 0.0);

    while(!pq_vazia(pq)){
        int atual= remove_min(pq);

        if(atual == -1) break;

        if(visitados[atual]) continue;

        visitados[atual]= true;

        Cano *cano= g->list_adj[atual];
        while(cano != NULL){
            if(cano->eh_reversa == true){
                cano= cano->proximo;
                continue;
            }
            int vizinho= cano->destino;
            float resistencia_cano= cano->resistencia;
            float nova_dist= distancias[atual] + resistencia_cano;

            if(nova_dist < distancias[vizinho]){    
                distancias[vizinho]= nova_dist;
                predecessor[vizinho]= atual;
                insere(pq, vizinho, nova_dist);
            }
            cano= cano->proximo;
        }
    }
    destroy_pq(pq);
    free(visitados);
}

void BFS(Graph *g, int origem, int *predecessor){
    Fila *f= criarFila(g->numnodes);
    bool *visitados= (bool*)calloc(g->numnodes, sizeof(bool));

    for(int i=0; i<g->numnodes; i++){
        predecessor[i]= -1;
    }

    visitados[origem]= true;
    enfileirar(f, origem);

    while(!filaVazia(f)){
        int atual= desenfileirar(f);
        
        if(atual == -1) break;

        Cano *cano= g->list_adj[atual];
        while(cano != NULL){
            if(cano->eh_reversa == true){
                cano= cano->proximo;
                continue;
            }
            int vizinho= cano->destino;

            if(visitados[vizinho] == false){
                visitados[vizinho] = true;
                predecessor[vizinho] = atual;

                enfileirar(f, vizinho);
            }
            cano= cano->proximo;
        }
    }
    encerrarFila(f);
    free(visitados);
}

bool *alcancaveis(Graph *g, int origem){
    Fila *f= criarFila(g->numnodes);

    bool *visitados= (bool*)calloc(g->numnodes, sizeof(bool));
    visitados[origem]= true;
    enfileirar(f, origem);

    while(!filaVazia(f)){
        int atual= desenfileirar(f);
        Cano *cano= g->list_adj[atual];

        while(cano != NULL){
            if(cano->eh_reversa == true){
                cano= cano->proximo;
                continue;
            }
            int vizinho= cano->destino;
            if(visitados[vizinho] == false){
                visitados[vizinho] = true;
                enfileirar(f, vizinho);
            }
            cano= cano->proximo;
        }
    }
    encerrarFila(f);
    return visitados;
}


void imprimir_caminho_pilha(Graph *g, int destino, int *predecessor){
    Pilha *p= create_pilha(g->numnodes);
    push(p, destino);

    int proximo= predecessor[destino];
    while(proximo != -1){
        push(p, proximo);
        proximo= predecessor[proximo];
    }

    while(!pilha_vazia(p)){
        printf("%d -> ", pop(p));
    }
    
    encerrarPilha(p);
}

void imprimir_arestas_prim(Graph *g, int *predecessor){
    
    printf("Arestas da Arvore Geradora Minima (Prim):\n");

    for(int i = 0; i < g->numnodes; i++) {
        if(predecessor[i] != -1) {
            printf("%d -> %d\n", predecessor[i], i);
        }
    }
}

float get_resistencia(Graph *g, int from_node, int to_node){
    Cano *cano= g->list_adj[from_node];

    while(cano != NULL){
        if(cano->eh_reversa == true){
            cano= cano->proximo;
            continue;
        }
        if(cano->destino == to_node){
            return cano->resistencia;
        }
        cano= cano->proximo;
    }
    return -1.0;
}

void dfs_recursive(Graph *g, int origem, int destino, bool *visitados, Pilha *p){
    if(origem == destino){
        push(p, origem);
        print_pilhaContrario(p);
        printf("\n");
        pop(p);
        return;
    }
    push(p, origem);
    visitados[origem]= true;
    Cano *cano= g->list_adj[origem];
    while(cano != NULL){
        if(cano->eh_reversa == true){
            cano= cano->proximo;
            continue;
        }
        if(visitados[cano->destino] == false){
            dfs_recursive(g, cano->destino, destino, visitados, p);
        }
        cano= cano->proximo;
    }
    visitados[origem]= false;
    pop(p);

    return;
}

void dfs_abb(Graph *g, int origem, int destino, bool *visitados, Pilha *p, Arvore *abb, float custo_total){
    if(origem == destino){
        push(p, origem);
        int tamanho= pilha_get_tamanho(p);
        if(tamanho <= 0){
            printf("Caminho inválido (tamanho 0).\n");
            pop(p);
            return;
        }
        int *caminho= (int*)calloc(tamanho, sizeof(int));
        if(caminho == NULL){
            printf("Nao conseguiu alocar memória para o caminho.\n");
            pop(p);
            return;
        }
        adicionar_caminhos(p, caminho);
        adicionar(abb, custo_total, caminho, tamanho);
        pop(p);
        return;
    }
    push(p, origem);
    visitados[origem]= true;
    Cano *cano= g->list_adj[origem];
    while(cano != NULL){
        if(cano->eh_reversa == true){
            cano= cano->proximo;
            continue;
        }
        if(visitados[cano->destino] == false){
            float novo_custo= custo_total + cano->resistencia;
            dfs_abb(g, cano->destino, destino, visitados, p, abb, novo_custo);
        }
        cano= cano->proximo;
    }
    visitados[origem]= false;
    pop(p);

    return;
}

void prim(Graph *g, int origem, float *distancias, int *predecessor){
    FilaPrio *pq= create_pq(g->numnodes * g->numnodes);
    bool *visitados= (bool*)calloc(g->numnodes, sizeof(bool));

    for(int i=0; i<g->numnodes; i++){
        distancias[i]= FLT_MAX;
        predecessor[i]= -1;
        visitados[i]= false;
    }
    distancias[origem]= 0.0;
    insere(pq, origem, 0.0);

    while(!pq_vazia(pq)){
        int atual= remove_min(pq);

        if(atual == -1) break;

        if(visitados[atual]) continue;

        visitados[atual]= true;

        Cano *cano= g->list_adj[atual];
        while(cano != NULL){
            if(cano->eh_reversa == true){
                cano= cano->proximo;
                continue;
            }
            int vizinho= cano->destino;
            float resistencia_cano= cano->resistencia;

            if(!visitados[vizinho] && resistencia_cano < distancias[vizinho]){
                distancias[vizinho]= resistencia_cano;
                predecessor[vizinho]= atual;
                insere(pq, vizinho, resistencia_cano);
            }
            cano= cano->proximo;
        }
    }

    destroy_pq(pq);
    free(visitados);
}


void DFS(Graph *g, int origem, int destino, int qual, Arvore *abb){
    Pilha *p= create_pilha(g->numnodes);
    bool *visitados= (bool*)calloc(g->numnodes, sizeof(bool));
    if(qual == 1){
        dfs_recursive(g, origem, destino, visitados, p);
    }else if(qual == 2){
        dfs_abb(g, origem, destino, visitados, p, abb, 0);
    }
    encerrarPilha(p);
    free(visitados);
    printf("\n");
}

int busca_caminho_ford_fukerson(Graph *g, int origem, int destino, int *predecessor, FilaPrio *pq){
    for(int i=0; i<g->numnodes; i++){
        predecessor[i]= -1;
    }
    int *fluxo_max_ate_no= (int*)calloc(g->numnodes, sizeof(int));
    if(fluxo_max_ate_no == NULL) return 0;

    fluxo_max_ate_no[origem]= -INT_MAX;
    insere(pq, origem, -INT_MAX);
    while(!pq_vazia(pq)){
        int removido= remove_min(pq);
        Cano *cano= g->list_adj[removido];
        while(cano != NULL){
            int vizinho= cano->destino;
            int capacidade_total= fluxo_max_ate_no[removido];
            int cap_residual= -cano->capacidade + cano->fluxo;
            int gargalo= max(cap_residual, capacidade_total);

            if(cap_residual < 0 && gargalo < fluxo_max_ate_no[vizinho]){
                fluxo_max_ate_no[vizinho]= gargalo;
                predecessor[vizinho]= removido;
                insere(pq, vizinho, fluxo_max_ate_no[vizinho]);
            }
            cano= cano->proximo;
        }
    }
    free(fluxo_max_ate_no);
    return (predecessor[destino] != -1);
}

int ford_fukerson(Graph *g, int origem, int destino, int contar){
    int fluxo_total= 0;
    int iteracao= 1;

    int *predecessor= (int*)malloc(g->numnodes * sizeof(int));
    FilaPrio *pq= create_pq(g->numnodes * g->numnodes);

    zerar_fluxos(g);

    while(busca_caminho_ford_fukerson(g, origem, destino, predecessor, pq)){
        if(contar == 1){
            printf("\nCaminho de Aumento #%d: ", iteracao++);
            Pilha *p = create_pilha(g->numnodes);
            int rastro = destino;
            while(rastro != -1){
                push(p, rastro);
                if(rastro == origem) break;
                rastro = predecessor[rastro];
            }
            while(!pilha_vazia(p)){
                printf("%d", pop(p));
                if(!pilha_vazia(p)){
                    printf(" -> ");
                }
            }
            encerrarPilha(p);
        }

        int atual= destino;
        int gargalo_caminho= INT_MAX;

        while(atual != origem){
            int anterior= predecessor[atual];
            Cano *cano= g->list_adj[anterior];
            while(cano != NULL){
                if(cano->destino == atual){
                    int cap_res= cano->capacidade - cano->fluxo;
                    if(cap_res < gargalo_caminho){
                        gargalo_caminho = cap_res;
                    }
                    break;
                }
                cano= cano->proximo;
            }
            atual= anterior;
        }
        if(gargalo_caminho <= 0 || gargalo_caminho == INT_MAX) break;
        if(contar == 1) printf(" [Gargalo: %d]\n", gargalo_caminho);

        atual= destino;
        while(atual != origem){
            int anterior= predecessor[atual];
            Cano *cano= g->list_adj[anterior];
            while(cano != NULL){
                if(cano->destino == atual){
                    cano->fluxo += gargalo_caminho;
                    break;
                }
                cano= cano->proximo;
            }
            Cano *cano_rev= g->list_adj[atual];
            while(cano_rev != NULL){
                if(cano_rev->destino == anterior){
                    cano_rev->fluxo -= gargalo_caminho;
                    break;
                }
                cano_rev= cano_rev->proximo;
            }
            atual= anterior;
        }
        fluxo_total += gargalo_caminho;
    }
    if(contar == 1){
        printf("Fluxo Maximo Calculado: %d\n", fluxo_total);
    }
    return fluxo_total;
}

void zerar_fluxos(Graph *g){
    for(int i=0; i<g->numnodes; i++){
        Cano *cano= g->list_adj[i];
        while(cano != NULL){
            cano->fluxo= 0;
            cano= cano->proximo;
        }
    }
}

void analisar_corte_agua(Graph *g, int origem, int cano_from, int cano_to){
    
    float resistencia_cano= get_resistencia(g, cano_from, cano_to);
    if(resistencia_cano == -1.0){
        return;
    }

    printf("\n--- Analisando Corte de Agua no Cano (%d -> %d) ---\n", cano_from, cano_to);

    bool *alcancaveis_antes= alcancaveis(g, origem);

    printf("Removendo o cano %d -> %d...\n", cano_from, cano_to);
    Cano *cano= g->list_adj[cano_from];
    while(cano != NULL){
        if(cano->eh_reversa == true){
            cano= cano->proximo;
            continue;
        }
        if(cano->destino == cano_to) break;
        cano= cano->proximo;
    }
    int capacidade= cano->capacidade;
    bool eh_reversa= cano->eh_reversa;
    remove_connect(g, cano_from, cano_to);

    bool* alcancaveis_depois = alcancaveis(g, origem);
    printf("Relatorio de Abastecimento:\n");
    bool algum_corte = false;
    for(int i=0; i<g->numnodes; i++){
        if(alcancaveis_antes[i] == true && alcancaveis_depois[i] == false){
            printf("O No %d (%s) ficou sem agua!\n", i, g->vertices[i].nome);
            algum_corte= true;
        }
    }
    if(!algum_corte){
        printf("Nenhum no perdeu o abastecimento (provavelmente existe uma rota alternativa).\n");
    }
    
    add_edge(g, cano_from, cano_to, resistencia_cano, capacidade, eh_reversa);

    free(alcancaveis_antes);
    free(alcancaveis_depois);
}


void exportar_json(Graph *g, int* pred_dijkstra, int* pred_bfs, int destino, int max_flow){
    FILE *f = fopen("vis/saida.json", "w");
    if (f == NULL) return;

    fprintf(f, "{\n");
    
    fprintf(f, "  \"nodes\": [\n");
    for (int i = 0; i < g->numnodes; i++){
        char *color = (g->vertices[i].tipo == TIPO_RESERVATORIO) ? "#97C2FC" : "#E0E0E0";
        fprintf(f, "    {\"id\": %d, \"label\": \"%s\\n(Alt: %.1f)\", \"color\": \"%s\"}%s\n", 
                i, g->vertices[i].nome, g->vertices[i].altura, color, 
                (i == g->numnodes - 1) ? "" : ",");
    }
    fprintf(f, "  ],\n");

    
    fprintf(f, "  \"edges\": [\n");
    int primeiro = 1;
    for (int i = 0; i < g->numnodes; i++){
        Cano *cano = g->list_adj[i];
        while (cano != NULL){
            if(cano->eh_reversa == true){
                cano= cano->proximo;
                continue;
            }
            if (!primeiro) fprintf(f, ",\n");

            char *cor_aresta= (cano->fluxo > 0) ? "#0044cc" : "#848484";

            int espessura= (cano->fluxo > 0) ? (cano->fluxo / 2) + 3 : 1;
            if(espessura > 10) espessura = 10;

            fprintf(f, "    {\"from\": %d, \"to\": %d, \"resistance_val\": %.2f, \"flow_val\": %d, \"capacity_val\": %d, \"label\": \"R: %.1f\\nF: %d/%d\", \"color\": {\"color\":\"%s\"}, \"width\": %d, \"id\": \"%d-%d\"}", 
                i, cano->destino, 
                cano->resistencia, 
                cano->fluxo,       
                cano->capacidade,  
                cano->resistencia,             
                cano->fluxo, cano->capacidade, 
                cor_aresta, 
                espessura, 
                i, cano->destino);
                
            primeiro = 0;
            cano = cano->proximo;
        }
    }
    fprintf(f, "\n  ],\n");

    fprintf(f, "  \"pred_dijkstra\": [");
    for(int i=0; i < g->numnodes; i++) fprintf(f, "%d%s", pred_dijkstra[i], (i<g->numnodes-1)?",":"");
    fprintf(f, "],\n");

    fprintf(f, "  \"pred_bfs\": [");
    for(int i=0; i < g->numnodes; i++) fprintf(f, "%d%s", pred_bfs[i], (i<g->numnodes-1)?",":"");
    fprintf(f, "],\n");

    fprintf(f, "  \"destino_escolhido\": %d,\n", destino);

    fprintf(f, "  \"max_flow\": %d\n", max_flow);

    fprintf(f, "}\n");
    fclose(f);
    printf("Arquivo 'saida.json' gerado com sucesso!\n");
}
