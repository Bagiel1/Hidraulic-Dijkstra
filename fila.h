typedef struct fila Fila;

// .h do TAD Fila

Fila *criarFila(int tamanho); // cria Fila, com vetor de itens com um tamanho máx

void enfileirar(Fila *f, int id); // adiciona no fim

int desenfileirar(Fila *f); // remove do começo

void encerrarFila(Fila *f); // libera a fila

int filaVazia(Fila *f); // verifica se a fila está vazia