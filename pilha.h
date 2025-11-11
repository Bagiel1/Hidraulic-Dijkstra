typedef struct pilha Pilha;

Pilha* create_pilha();

void push(Pilha* pilha, float valor); // empilha

float pop(Pilha* pilha); // desempilha
 
void encerrarPilha(Pilha* pilha); // imprimir todo conteudo da pilha, do topo para base, e encerrar

int pilha_vazia(Pilha* pilha);

