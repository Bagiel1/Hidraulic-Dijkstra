#include <stdio.h>
#include <stdlib.h>
#include "fila.h"

struct fila{
    int *ids; // vetor que guarda elementos
    int inicio, final, count; // indice do primeiro elemento, do último + 1 (para acrescentar), e a quantidade de elementos
    int tamanho_max; // capacidade
};

int filaCheia(Fila *f){
    return(f->count == f->tamanho_max); // se quantidade de elementos é igual a capacidade máxima da fila
}
int filaVazia(Fila *f){
    return(f->count == 0); // se quantidade de elementos atual é nula
}

Fila *criarFila(int tamanho){
    Fila *f= (Fila*)malloc(sizeof(Fila)); // aloca um bloco de memória que o ponteiro aponta
    f->ids= (int*)calloc(tamanho, sizeof(int)); // cria vetor de inteiros, com 0´s (reinicia)
    if(f == NULL){
        return NULL;
    }
    f->final= 0; // iniciando valores
    f->inicio= 0;
    f->count= 0;
    f->tamanho_max= tamanho;

    return f;
}

void enfileirar(Fila *f, int id){
    if(filaCheia(f)){ // verifica se está cheia
        return;
    }
    f->ids[f->final]= id;  // para acrescentar na fila  
    f->final= (f->final+1)%f->tamanho_max;  // para que o final voltar para o começo, fila circular
    f->count++; // +1 na quantidade
}

int desenfileirar(Fila *f){
    if(filaVazia(f)){
        printf("Fila vazia");
        return -1; // verifica se está vazia
    }
    int id= f->ids[f->inicio]; // para remover do inicio
    f->inicio= (f->inicio+1)%f->tamanho_max; // mesma coisa do enfileirar, mas ao contrário
    f->count--; // -1 na quantidade
    return id;
}


void encerrarFila(Fila *f){ // libera fila
    free(f->ids);
    free(f);
}