#include <stdio.h>
#include <stdlib.h>
#include "fila.h"

#define TAM_MAX 50

struct fila{
    int ids[TAM_MAX];
    int inicio, final, count;
};

Fila *criarFila(){
    Fila *f= (Fila*)malloc(sizeof(Fila));
    if(f == NULL){
        return NULL;
    }
    f->final= 0;
    f->inicio= 0;
    f->count= 0;

    return f;
}

void enfileirar(Fila *f, int id){
    if(filaCheia(f)){
        return;
    }
    f->ids[f->final]= id;  
    f->final= (f->final+1)%TAM_MAX;
    f->count++; 
}

int desenfileirar(Fila *f){
    if(filaVazia(f)){
        printf("Fila vazia");
        return 0;
    }
    int id= f->ids[f->inicio];
    f->inicio= (f->inicio+1)%TAM_MAX;
    f->count--;
    return id;
}

int filaCheia(Fila *f){
    return(f->count == TAM_MAX);
}
int filaVazia(Fila *f){
    return(f->count == 0);
}

void encerrarFila(Fila *f){
    free(f);
}