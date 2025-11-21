typedef struct pilha Pilha; // estrutura para pilha

// arquivo .h do TAD Pilha

Pilha* create_pilha(); // cria pilha

void push(Pilha* pilha, int valor); // empilha

int pop(Pilha* pilha); // desempilha
 
void encerrarPilha(Pilha* pilha); // imprimir todo conteudo da pilha, do topo para base, e encerrar

int pilha_vazia(Pilha* pilha); // verifica se a pilha está vazia

void print_pilhaContrario(Pilha *pilha); // printa a pilha do fundo ao topo

int pilha_get_tamanho(Pilha *p); // retorna n° de elementos da pilha

void adicionar_caminhos(Pilha *p, int *caminhos); // armazena conteúdo da pilha num vetor

