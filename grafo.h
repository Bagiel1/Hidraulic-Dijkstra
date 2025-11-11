#include <stdbool.h>

typedef enum {
    TIPO_JUNÇÃO,
    TIPO_RESERVATORIO
} TipoVertice;

struct reservatorio {
    float capacidade;
    float atual;
};
typedef struct reservatorio Reservatorio; 

struct dadosjuncao {
    int tipoJuncao;
};
typedef struct dadosjuncao DadosJuncao; 

struct vertice {
    int id;
    float altura;
    char *nome;
    TipoVertice tipo;   

    union {
        Reservatorio reservatorio;
        DadosJuncao juncao;
    } dados;
};
typedef struct vertice Vertice; 

struct cano {
    int destino;
    float resistencia;
    struct cano *proximo; 
};
typedef struct cano Cano; 

struct graph {
    int numnodes;
    Vertice *vertices;
    Cano **list_adj;
};
typedef struct graph Graph; 


Graph *create_graph(int numnodes);
void destroy_graph(Graph *g);
void print_graph(Graph *g);
void remove_connect(Graph *g, int from_node, int to_node);
int hasEdge(Graph *g, int from_node, int to_node);
int set_data(Graph *g, int id, char *nome, float altura);
void add_cano_com_altura(Graph *g, int id_A, int id_B, float resistencia);
void djisktra(Graph *g, int origem, float *distancias, int *predecessor);
void BFS(Graph *g, int origem, int *predecessor);
bool *alcancaveis(Graph *g, int origem);
void analisar_corte_agua(Graph *g, int origem, int cano_from, int cano_to);
void imprimir_caminho_pilha(Graph *g, int destino, int *predecessor);