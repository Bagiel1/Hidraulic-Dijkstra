
typedef struct graph Graph;
typedef struct vertice Vertice;
typedef struct reservatorio Reservatorio;
typedef struct dadosjuncao DadosJuncao;

Graph *create_graph(int numnodes);
void destroy_graph(Graph *g);
void print_graph(Graph *g);
int add_edge(Graph *g, int from_node, int to_node, float resistencia);
int hasEdge(Graph *g, int from_node, int to_node);