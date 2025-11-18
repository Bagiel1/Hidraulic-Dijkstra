typedef struct arvore Arvore;
typedef struct no No;
typedef struct produto Produto;

Arvore* criarABB();
int adicionar(Arvore *arvore, float custo, int *caminho, int tamanho);
No *buscar(Arvore *arvore, float custo);
void encerrarArvore(Arvore *arvore);
int *get_caminho_do_no(No *no);
int get_tamanho_do_no(No *no);