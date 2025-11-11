typedef struct pilha Pilha;

Pilha* create();

void push(Pilha* pilha, float valor); // empilha

float pop(Pilha* pilha); // desempilha
 
float top(Pilha* pilha); // valor do topo

float soma(Pilha* pilha); // desempilha 2, empilha e imprime o resultado

float subtracao(Pilha* pilha); // desempilha B, depois A, e calcula A - B, empilha e
                // imprime o resultado

float multiplicacao(Pilha* pilha); // desempilha 2 deles, empilha e imprime o resultado

float divisao(Pilha* pilha); // desempilha B, depois A, e calcula A/B, empilha e 
                // imprime o resultado

float potencia(Pilha* pilha); // desempilha B, depois A, e calcula A^B, empilha e 
                // imprime o resultado

void encerrar(Pilha* pilha); // imprimir todo conteudo da pilha, do topo para base, e encerrar



