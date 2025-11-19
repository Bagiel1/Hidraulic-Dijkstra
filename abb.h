typedef struct arvore Arvore;
typedef struct no No;
typedef struct produto Produto;
typedef struct caminho_item CaminhoItem;

Arvore* criarABB();
int adicionar(Arvore *arvore, float custo, int *caminho, int tamanho);
No *buscar(Arvore *arvore, float custo);
void encerrarArvore(Arvore *arvore);
void buscarIntervalo(Arvore *arvore, float min, float max);