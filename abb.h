typedef struct arvore Arvore;
typedef struct no No;
typedef struct produto Produto;
typedef struct caminho_item CaminhoItem; // define as estruturas

Arvore* criarABB(); // cria Arvore Binaria de Busca
int adicionar(Arvore *arvore, float custo, int *caminho, int tamanho); // insere nó
No *buscar(Arvore *arvore, float custo); // busca um caminho pelo custo
void encerrarArvore(Arvore *arvore); // Encerra e libera
void buscarIntervalo(Arvore *arvore, float min, float max); // busca caminhos num intervalo de custo