#include <stdio.h>
#include <stdlib.h>
#include "fila.h"

struct fila{
    int *ids;
    int inicio, final, count;
    int tamanho_max;
};

Fila *criarFila(int tamanho){
    Fila *f= (Fila*)malloc(sizeof(Fila));
    f->ids= (int*)calloc(tamanho, sizeof(int));
    if(f == NULL){
        return NULL;
    }
    f->final= 0;
    f->inicio= 0;
    f->count= 0;
    f->tamanho_max= tamanho;

    return f;
}

void enfileirar(Fila *f, int id){
    if(filaCheia(f)){
        return;
    }
    f->ids[f->final]= id;  
    f->final= (f->final+1)%f->tamanho_max;
    f->count++; 
}

int desenfileirar(Fila *f){
    if(filaVazia(f)){
        printf("Fila vazia");
        return -1;
    }
    int id= f->ids[f->inicio];
    f->inicio= (f->inicio+1)%f->tamanho_max;
    f->count--;
    return id;
}

int filaCheia(Fila *f){
    return(f->count == f->tamanho_max);
}
int filaVazia(Fila *f){
    return(f->count == 0);
}

void encerrarFila(Fila *f){
    free(f->ids);
    free(f);
}