#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "grafo.h"

typedef enum{
    TIPO_JUNÇÃO,
    TIPO_RESERVATORIO
} TipoVertice;

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
    float **edges;
};


Graph *create_graph(int numnodes){
    Graph *g= (Graph*)malloc(sizeof(Graph));
    if(g == NULL){
        return NULL;
    }

    g->numnodes= numnodes;
    g->edges= calloc(sizeof(float*), g->numnodes);
    g->vertices= calloc(sizeof(Vertice), g->numnodes);
    if(g->edges == NULL || g->vertices == NULL){
        free(g->vertices);
        free(g->edges);
        free(g);
        return NULL;
    }

    for(int i=0; i<g->numnodes; i++){
        g->edges[i]= calloc(sizeof(float), g->numnodes);
        if(g->edges[i] == NULL){
            destroy_graph(g);
            return NULL;
        }
    }
    
    for(int i=0; i<numnodes; i++){
        g->vertices[i].id= i;
        g->vertices[i].nome= NULL;
    }

    return g;

}

int set_data(Graph *g, int id, char nome[], float altura){
    if(g == NULL || id < 0 || id >= g->numnodes){
        return 0;
    }

    Vertice *v= &g->vertices[id];
    v->altura= altura;

    if(v->nome != NULL){
        free(v->nome);
    }

    v->nome= (char*)malloc(strlen(nome)+1);
    if(v->nome == NULL){
        return 0;
    }

    strcpy(v->nome, nome);
    return 1;
}

void destroy_graph(Graph *g){
    if(g == NULL){
        return;
    }

    if(g->edges != NULL){
        for(int i=0; i<g->numnodes; i++){
            if(g->edges[i] != NULL){
                free(g->edges[i]);
            }
        }
    free(g->edges);
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
        for(int to=0; to<g->numnodes; to++){
            if(g->edges[from][to] > 0){
                printf("%d -> %d, resistencia: %.1f\n", from, to, g->edges[from][to]);
            }
        }
    }
    printf("}\n");
}


int add_edge(Graph *g, int from_node, int to_node, float resistencia){

    if(g==NULL || from_node<0 || from_node>=g->numnodes || to_node < 0 || to_node >= g->numnodes){
        return 0;
    }
    if(hasEdge(g, from_node, to_node)){
        return 0;
    }

    g->edges[from_node][to_node]= resistencia;
    
    return 1;
}

int hasEdge(Graph *g, int from_node, int to_node){
    return g->edges[from_node][to_node] > 0;
}