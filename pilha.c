#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include"pilha.h"

#define TAM_PILHA 100

struct pilha{
    int topo;
    int *itens;
    int capacidade;
};

// aloca um bloco de memória dinamicamente (com tamanho da pilha)
// para um ponteiro do tipo da pilha apontar, 
// inicializando topo = -1 (pilha vazia)
Pilha* create_pilha(int capacidade) {
    Pilha* p = (Pilha*)malloc(sizeof(Pilha));
    if (p == NULL) {
        printf("Erro ao alocar memória para a pilha.\n");
        exit(1); // verificação de segurança
    }
    p->itens= (int*)malloc(capacidade * sizeof(int)); // aloca memória para o vetor de itens
    if(p->itens == NULL){
        free(p);
        exit(1);
    }
    p->capacidade= capacidade;
    p->topo= -1;
    return p;
}

// empilha os termos (coloca no topo)
void push(Pilha* pilha, int id) {
    if(pilha->topo + 1 >= pilha->capacidade){
        printf("ERRO: Estouro de Pilha\n");
        return;
    }

    pilha->topo++;
    pilha->itens[pilha->topo] = id;

}

// retira o termo do topo e imprime o valor
int pop(Pilha* pilha) {
    if(pilha_vazia(pilha)){
        printf("ERRO: Pilha vazia\n");
        return -1;
    }
    int id = pilha->itens[pilha->topo];
    pilha->topo--;

    return id;
}

int pilha_vazia(Pilha* pilha){
    return pilha->topo==-1; // se o topo é -1, a pilha está vazia
}

void print_pilhaContrario(Pilha *pilha){
    for(int i=0; i<=pilha->topo; i++){
        printf("%d", pilha->itens[i]);
        if(i < pilha->topo){
            printf(" -> ");
        }
    }
} // imprimi do fundo ao topo da pilha

// encerra e imprime todos os valores da pilha (do topo até a base)
void encerrarPilha(Pilha* pilha) {
    if(pilha == NULL) return;
    free(pilha->itens);
    free(pilha);
}


int pilha_get_tamanho(Pilha *p){
    if(pilha_vazia(p)){
        return 0; // verifica se a pilha está vazia
    }
    return p->topo + 1; // retorna tamanho da pilha
}


void adicionar_caminhos(Pilha *p, int *caminhos){ // *caminhos é o vetor onde vamos adicionar os valores
    if(p == NULL || p->itens == NULL || caminhos == NULL){
        printf("[ERRO] Tentativa de copiar pilha invalida ou vetor nulo!\n");
        return; // verifica se o ponteiro que aponta para a pilha ou para os itens é inexistente
    }

    for(int i=0; i <= p->topo; i++){
        caminhos[i] = p->itens[i]; // armazenando no vetor
    }
}
