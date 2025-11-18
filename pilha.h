typedef struct pilha Pilha;

Pilha* create_pilha();

void push(Pilha* pilha, int valor); // empilha

int pop(Pilha* pilha); // desempilha
 
void encerrarPilha(Pilha* pilha); // imprimir todo conteudo da pilha, do topo para base, e encerrar

int pilha_vazia(Pilha* pilha);

void print_pilhaContrario(Pilha *pilha);

int pilha_get_tamanho(Pilha *p);

void adicionar_caminhos(Pilha *p, int *caminhos);

