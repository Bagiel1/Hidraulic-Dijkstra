typedef struct item Item;
typedef struct filaprio{
    Item *heap; // um ponteiro para um vetor que guarda os itens, menor elemento no índice 0 (elemento de menor prioridade)
    int tam_atual; // tamanho atual
    int tam_max; // tamanho máx
} FilaPrio;


FilaPrio *create_pq(int tam_max); // cria fila de prioridade com um espaço máximo
void destroy_pq(FilaPrio *pq); // libera fila de prioridade
int pq_vazia(FilaPrio *pq); // verifica se está vazia
void insere(FilaPrio *pq, int id, float prioridade); // insere na fila de prioridade, com um determinado identificador e prioridade
int remove_min(FilaPrio *pq); // remove o item com menor prioridade