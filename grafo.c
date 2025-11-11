#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <float.h>
#include "grafo.h"
#include "pq.h"
#include "fila.h"
#include "pilha.h"

typedef enum{
    TIPO_JUNÇÃO,
    TIPO_RESERVATORIO
} TipoVertice;

struct cano{
    int destino;
    float resistencia;
    struct Cano *proximo;
};

struct reservatorio{
    float capacidade;
    float atual;
};

struct dadosjuncao{
    int tipoJuncao;
};

struct vertice{
    int id;
    float altura;
    char *nome;
    TipoVertice tipo;   

    union{
        Reservatorio reservatorio;
        DadosJuncao juncao;
    }dados;
};

struct graph{
    int numnodes;
    Vertice *vertices;
    Cano **list_adj;
};


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
            printf("%d -> %d, resistencia: %.1f\n", from, cano->destino, cano->resistencia);
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

int add_edge(Graph *g, int from_node, int to_node, float resistencia){

    if(g==NULL || from_node<0 || from_node>=g->numnodes || to_node < 0 || to_node >= g->numnodes){
        return 0;
    }
    if(hasEdge(g, from_node, to_node)){
        return 0;
    }

    

    Cano *cano_novo= (Cano*)malloc(sizeof(Cano));
    cano_novo->destino= to_node;
    cano_novo->proximo= g->list_adj[from_node];
    cano_novo->resistencia= resistencia;
    g->list_adj[from_node]= cano_novo;
    
    return 1;
}

int hasEdge(Graph *g, int from_node, int to_node){
    Cano *cano= g->list_adj[from_node];
    while(cano != NULL){
        if(cano->destino == to_node){
            return 1;
        }
        cano= cano->proximo;
    }
    return 0;
}

void add_cano_com_altura(Graph *g, int id_A, int id_B, float resistencia){
    float altura_A= g->vertices[id_A].altura;
    float altura_B= g->vertices[id_B].altura;

    if(altura_A > altura_B){
        add_edge(g, id_A, id_B, resistencia);
    }else if(altura_B > altura_B){
        add_edge(g, id_B, id_A, resistencia);
    }else{
        add_edge(g, id_A, id_B, resistencia);
        add_edge(g, id_B, id_A, resistencia);
    }
}

void djisktra(Graph *g, int origem, float *distancias, int *predecessor){
    FilaPrio *pq= create_pq(g->numnodes);
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


void imprimir_caminho_djisktra(Graph *g, int destino, int *predecessor){
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
    

}