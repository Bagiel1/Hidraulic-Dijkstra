typedef struct fila Fila;

Fila *criarFila(int tamanho);
void enfileirar(Fila *f, int id);
int desenfileirar(Fila *f);
void encerrarFila(Fila *f);
int filaVazia(Fila *f);