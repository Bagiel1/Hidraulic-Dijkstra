typedef struct item Item;
typedef struct filaprio{
    Item *heap;
    int tam_atual;
    int tam_max;
} FilaPrio;


FilaPrio *create_pq(int tam_max);
void destroy_pq(FilaPrio *pq);
int pq_vazia(FilaPrio *pq);
void insere(FilaPrio *pq, int id, float prioridade);
int remove_min(FilaPrio *pq);