
typedef struct graph Graph;
typedef struct vertice Vertice;
typedef struct reservatorio Reservatorio;
typedef struct dadosjuncao DadosJuncao;
typedef struct cano Cano;

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
void imprimir_caminho_djisktra(Graph *g, int destino, int *predecessor);