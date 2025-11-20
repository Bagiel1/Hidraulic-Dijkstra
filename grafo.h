#include <stdbool.h>
#include "abb.h"

// arquivo .h do TAD Grafo

typedef enum {
    TIPO_JUNÇÃO, // 0
    TIPO_RESERVATORIO // 1
} TipoVertice; // característica que determinado vértice assume

struct reservatorio {
    float capacidade; // valor que ele carrega (sua capacidade)
};
typedef struct reservatorio Reservatorio; 

struct dadosjuncao {
    int tipoJuncao; // valor que ele carrega (genérico)
};
typedef struct dadosjuncao DadosJuncao; 

struct vertice {
    int id; // valor de identificação
    float altura; // altura do nó
    char *nome; // ponteiro para string do nó
    TipoVertice tipo; // TIPO_JUNCAO ou TIPO_RESERVATORIO

    union { // vértice é reservatório ou junção
        Reservatorio reservatorio; // guarda capacidade ou tipoJuncao
        DadosJuncao juncao;
    } dados};// union faz com que as variáveis compartilhem o mesmo espaço de memória
typedef struct vertice Vertice; 

// maior parte das características está no cano (conexão)

struct cano {
    int destino; // vértice para onde o cano vai
    float resistencia; // peso da conexão
    struct cano *proximo; // ponteiro que aponta para a próxima aresta na lista de adjacência
};
typedef struct cano Cano; 

struct graph {
    int numnodes; // número de nós do grafo
    Vertice *vertices; // ponteiro para a lista de vértices
    Cano **list_adj; // ponteiro de ponteiro para da lista de adjacencia
    // cada posição tem um ponteiro que aponta para a conexão do vértice
};
typedef struct graph Graph; 


Graph *create_graph(int numnodes); // cria grafo com um determinado número de nós
void destroy_graph(Graph *g); // destroi grafo, limpa ele todo
void print_graph(Graph *g); // printa a situação do grafo
void remove_connect(Graph *g, int from_node, int to_node); // remove a conexão, passando o nó de origem e de destino
int hasEdge(Graph *g, int from_node, int to_node); // verifica se tem conexão entre dois nós
int set_data(Graph *g, int id, char *nome, float altura); // alterar o nó de id definido para um novo nome e altura
void add_cano_com_altura(Graph *g, int id_A, int id_B, float resistencia); // adiciona uma conexão com base na altura, para saber o sentido
void djisktra(Graph *g, int origem, float *distancias, int *predecessor); // algoritmo para achar o caminho de menor custo
void BFS(Graph *g, int origem, int *predecessor); // busca em largura, encontra caminho mais curto sem considerar pesos
// predecessor é uma lista que diz qual vértice vem antes do que estamos analisando na iteração, num caminho
bool *alcancaveis(Graph *g, int origem); // quais vértices podem ser encontrados a partir de um de origem
void analisar_corte_agua(Graph *g, int origem, int cano_from, int cano_to); // corta água num cano e vê quais vértices ficam sem abastecer
void imprimir_caminho_pilha(Graph *g, int destino, int *predecessor); // apenas para imprimir o caminho gerado por djisktra ao contrário
void DFS(Graph *g, int origem, int destino, int qual, Arvore *abb); // gera todos os caminhos entre dois vértices
void prim(Graph *g, int origem, float *distancias, int *predecessor); // conectar todos os nós com menor custo possível, sem ciclo
void imprimir_arestas_prim(Graph *g, int *predecessor); // imprime o caminho encontrado pelo algoritmo de prim
void exportar_json(Graph *g, int *pred_djisktra, int *pred_bfs, int destino); // exportar todo o grafo pra um arquivo jason, para visualização