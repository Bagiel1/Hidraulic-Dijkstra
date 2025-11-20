#include <stdio.h>
#include <stdlib.h>
#include "pq.h"


struct item{ //
    int id;
    float prioridade;
};


FilaPrio *create_pq(int tam_max){
    FilaPrio *pq= (FilaPrio*)malloc(sizeof(FilaPrio));
    if(pq == NULL){
        return NULL;
    }
    pq->heap= (Item*)calloc(tam_max, sizeof(Item));
    if(pq->heap == NULL){
        free(pq);
        return NULL;
    }
    pq->tam_atual= 0;
    pq->tam_max= tam_max;

    return pq;
}

void destroy_pq(FilaPrio *pq){
    free(pq->heap);
    free(pq);
}

int pq_vazia(FilaPrio *pq){
    return pq->tam_atual == 0;
}

void trocar(FilaPrio *pq, int i, int j){
    Item temp;
    temp= pq->heap[i];
    pq->heap[i]= pq->heap[j];
    pq->heap[j]= temp;
}

void subir(FilaPrio *pq, int i){
    int pai= (i-1)/2;
    while(i > 0 && pq->heap[i].prioridade < pq->heap[pai].prioridade){
        trocar(pq, i, pai);
        i=pai;
        pai= (i-1)/2;
    }
}

void descer(FilaPrio *pq, int i){
    int filho1, filho2, menor;
    while((i*2)+1 < pq->tam_atual){

        filho1= (i*2)+1;
        filho2= (i*2)+2;

        menor= filho1;

        if(filho2 < pq->tam_atual && pq->heap[filho2].prioridade < pq->heap[filho1].prioridade){
            menor= filho2;
        }

        if(pq->heap[i].prioridade > pq->heap[menor].prioridade){
            trocar(pq, i, menor);
            i= menor;
        }else{
            break;
        }
    }
}

void insere(FilaPrio *pq, int id, float prioridade){
    if(pq->tam_atual == pq->tam_max){
        return;
    }
    int i= pq->tam_atual;
    pq->heap[i].id= id;
    pq->heap[i].prioridade= prioridade;
    pq->tam_atual++;
    subir(pq, i);
}

int remove_min(FilaPrio *pq){
    if(pq_vazia(pq)) return -1;

    int id_removido= pq->heap[0].id;
    int id_ultimo= pq->tam_atual-1;
    pq->heap[0]= pq->heap[id_ultimo];
    pq->tam_atual--;
    descer(pq, 0);
    
    return id_removido;
}

