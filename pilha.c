#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include"pilha.h"

#define TAM_PILHA 100

struct pilha{
 int topo;
 int itens[TAM_PILHA];
};

// aloca um bloco de memória dinamicamente (com tamanho da pilha)
// para um ponteiro do tipo da pilha apontar, 
// inicializando topo = -1 (pilha vazia)
Pilha* create_pilha() {
    Pilha* p = (Pilha*)malloc(sizeof(Pilha));
    if (p == NULL) {
        printf("Erro ao alocar memória para a pilha.\n");
        exit(1);
    }
    p->topo = -1;
    return p;
}

// empilha os termos (coloca no topo)
void push(Pilha* pilha, int id) {
    pilha->topo++;
    pilha->itens[pilha->topo] = id;

}

// retira o termo do topo e imprime o valor
int pop(Pilha* pilha) {
    int id = pilha->itens[pilha->topo];
    pilha->topo--;

    return id;
}

int pilha_vazia(Pilha* pilha){
    return pilha->topo==-1;
}

// encerra e imprime todos os valores da pilha (do topo até a base)
void encerrarPilha(Pilha* pilha) {
    printf("Pilha: ");
    for (int i = pilha->topo; i >= 0; i--) {
        printf("%.2f ", pilha->itens[i]);
    }
    printf("\n");
}